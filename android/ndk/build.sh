ndk-build
echo
echo "* Boiler and game code built"
echo "* Copying shared objects into android-project"
cp libs/armeabi/* ../android-studio/app/src/main/jniLibs/armeabi/
echo
echo "* All done!"
