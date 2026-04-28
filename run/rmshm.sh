#!/bin/bash
# 删除指定用户权限为666的共享内存脚本

# 检查是否提供了用户名参数
if [ -z "$1" ]; then
    echo "Usage: $0 <username>"
    exit 1
fi

USERNAME="$1"
TARGET_PERMS="666"

# 检查用户是否存在
if ! id "$USERNAME" &>/dev/null; then
    echo "Error: User '$USERNAME' does not exist."
    exit 1
fi

echo "Deleting shared memory segments owned by user: $USERNAME with permissions: $TARGET_PERMS"

# 获取用户的所有权限为666的共享内存段ID
# ipcs -m 输出格式: key shmid owner perms bytes nattch status
# 第3列是owner，第4列是perms
SHMIDS=$(ipcs -m | awk -v user="$USERNAME" -v perms="$TARGET_PERMS" '$3 == user && $4 == perms {print $2}')

if [ -z "$SHMIDS" ]; then
    echo "No shared memory segments found for user '$USERNAME' with permissions '$TARGET_PERMS'."
    exit 0
fi

# 显示找到的共享内存段信息
echo "Found the following shared memory segments to delete:"
ipcs -m | awk -v user="$USERNAME" -v perms="$TARGET_PERMS" '$3 == user && $4 == perms {print $0}'

# 确认是否删除
read -p "Are you sure you want to delete these shared memory segments? (y/N) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Operation cancelled."
    exit 0
fi

# 删除每个共享内存段
for shmid in $SHMIDS; do
    echo "Removing shared memory segment: $shmid"
    ipcrm -m "$shmid"
    if [ $? -eq 0 ]; then
        echo "Successfully removed shared memory segment $shmid"
    else
        echo "Failed to remove shared memory segment $shmid"
    fi
done

echo "Done."