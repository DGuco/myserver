// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "spdlog/common.h"
#include "spdlog/details/file_helper.h"
#include "spdlog/details/null_mutex.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/details/os.h"
#include "spdlog/details/synchronous_factory.h"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <string>

namespace spdlog {
    namespace sinks {

        /*
         * Generator of hour log file names in format basename.YYYY-MM-DD.ext
         */
        struct hour_filename_calculator
        {
            // Create filename for the form basename.YYYY-MM-DD
            static filename_t calc_filename(const filename_t& filename, const tm& now_tm)
            {
                filename_t basename, ext;
                std::tie(basename, ext) = details::file_helper::split_by_extension(filename);
                return fmt::format(
                    SPDLOG_FILENAME_T("{}_{:04d}-{:02d}-{:02d}-{:02d}{}"), basename, now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday, now_tm.tm_hour, ext);
            }
        };

        /*
         * Rotating file sink based on date.
         * If truncate != false , the created file will be truncated.
         * If max_files > 0, retain only the last max_files and delete previous.
         */
        template<typename Mutex, typename FileNameCalc = hour_filename_calculator>
        class hour_file_sink final : public base_sink<Mutex>
        {
        public:
            // create daily file sink which rotates on given time
            hour_file_sink(filename_t base_filename, int roathour = 1, bool truncate = false, uint16_t max_files = 0)
                : base_filename_(std::move(base_filename))
                , rotation_h_(roathour)
                , truncate_(truncate)
                , max_files_(max_files)
                , filenames_q_()
            {
                auto now = log_clock::now();
                auto filename = FileNameCalc::calc_filename(base_filename_, now_tm(now));
                file_helper_.open(filename, truncate_);
                rotation_tp_ = next_rotation_tp_();

                if (max_files_ > 0)
                {
                    filenames_q_ = details::circular_q<filename_t>(static_cast<size_t>(max_files_));
                    filenames_q_.push_back(std::move(filename));
                }
            }

            const filename_t& filename() const
            {
                return file_helper_.filename();
            }

        protected:
            void sink_it_(const details::log_msg& msg) override
            {
#ifdef SPDLOG_NO_DATETIME
                auto time = log_clock::now();
#else
                auto time = msg.time;
#endif

                bool should_rotate = time >= rotation_tp_;
                if (should_rotate)
                {
                    auto filename = FileNameCalc::calc_filename(base_filename_, now_tm(time));
                    file_helper_.open(filename, truncate_);
                    rotation_tp_ = next_rotation_tp_();
                }
                memory_buf_t formatted;
                base_sink<Mutex>::formatter_->format(msg, formatted);
                file_helper_.write(formatted);

                // Do the cleaning ony at the end because it might throw on failure.
                if (should_rotate && max_files_ > 0)
                {
                    delete_old_();
                }
            }

            void flush_() override
            {
                file_helper_.flush();
            }

        private:
            tm now_tm(log_clock::time_point tp)
            {
                time_t tnow = log_clock::to_time_t(tp);
                return spdlog::details::os::localtime(tnow);
            }

            log_clock::time_point next_rotation_tp_()
            {
                auto now = log_clock::now();
                tm date = now_tm(now);
                date.tm_hour = 0;
                date.tm_min = 0;
                date.tm_sec = 0;
                auto rotation_time = log_clock::from_time_t(std::mktime(&date));
                if (rotation_time > now)
                {
                    return rotation_time;
                }
                return { rotation_time + std::chrono::hours(rotation_h_) };
            }

            // Delete the file N rotations ago.
            // Throw spdlog_ex on failure to delete the old file.
            void delete_old_()
            {
                using details::os::filename_to_str;
                using details::os::remove_if_exists;

                filename_t current_file = filename();
                if (filenames_q_.full())
                {
                    auto old_filename = std::move(filenames_q_.front());
                    filenames_q_.pop_front();
                    bool ok = remove_if_exists(old_filename) == 0;
                    if (!ok)
                    {
                        filenames_q_.push_back(std::move(current_file));
                        SPDLOG_THROW(spdlog_ex("Failed removing hour file " + filename_to_str(old_filename), errno));
                    }
                }
                filenames_q_.push_back(std::move(current_file));
            }

            filename_t base_filename_;
            int rotation_h_;
            log_clock::time_point rotation_tp_;
            details::file_helper file_helper_;
            bool truncate_;
            uint16_t max_files_;
            details::circular_q<filename_t> filenames_q_;
        };

        using hour_file_sink_mt = hour_file_sink<std::mutex>;
        using hour_file_sink_st = hour_file_sink<details::null_mutex>;

    } // namespace sinks
    //

    // factory functions
    //
    template<typename Factory = spdlog::synchronous_factory>
    inline std::shared_ptr<logger> hour_logger_mt(
        const std::string& logger_name, const filename_t& filename, int roathour = 1, bool truncate = false)
    {
        return Factory::template create<sinks::hour_file_sink_mt>(logger_name, filename, roathour,truncate);
    }

    template<typename Factory = spdlog::synchronous_factory>
    inline std::shared_ptr<logger> hour_logger_st(
        const std::string& logger_name, const filename_t& filename, int roathour = 1, bool truncate = false)
    {
        return Factory::template create<sinks::hour_file_sink_st>(logger_name, filename, roathour, truncate);
    }
} // namespace spdlog
