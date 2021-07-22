@del /Q obj\*.hex
@del /Q *.hex
set PATH=%PATH%;D:\Code\arduino-nightly\hardware\tools\avr\utils\bin
make all
copy obj\*.hex
dir *.hex
pause
