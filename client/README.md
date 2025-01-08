# A simple mobile client to read Sonde data over BLE

## Steps to create mobile application
1. Client was created using MIT App Inventor platoform
   To start create/loging to your account at https://appinventor.mit.edu/

2. Import project Sentry226.aia

3. The project is based on the example provided in this video tutorial
   https://youtu.be/RvbWl8rZOoQ?si=pZsIaVzMO03b3WGp

   However, version of BLE extension that is referenced there was outdated and instantly crashed Android application
   when attempting to connect to ESP device.
   Just in case, latest stable version of extension (already part of the project) is provided here for future reference.
   See edu.mit.appinventor.ble-20240822.aix

   Note: To add/update BLE extension to any appinventor project, Switch to Designer mode, scroll down to Extentions menu,
         expand it, click import extension and browse to *.aix file on your local drive.

   Note: Client application functionality was validated on Google Pixel 5, but in general should work on any Android device
         that supports BLE (requires Andriod 5 and up)
