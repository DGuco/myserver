del *.pb.*
cd proto\
for /r %%atomic_int1 in (*.proto) do (
	..\protoc.exe %%~ni%%~xi --cpp_out=..\
)
cd ..