# react-native-desktop
Sample using react-native on macOS and Windows

## Start

To clone and run this repository you'll need [Git](https://git-scm.com), [Node.js](https://nodejs.org/en/download/) (which comes with [npm](http://npmjs.com)) installed on your computer. 

### From your command line:

```bash
# Clone this repository
git clone https://github.com/christophpurrer/react-native-desktop.git
# Go into the repository
cd react-native-desktop
# Install dependencies
yarn install
# Download third party source code for macOS
node_modules/react-native/scripts/ios-install-third-party.sh 
# start Metro
yarn start
```
### Now open the Project in XCode ....

Trying to build *React-macOS* ...

![React-macOS](doc/buildReactmacOS.png)

you will encounter a cyclic dependency error:

![cyclic dependency error](doc/buildErrorCyclicDependency.png)

Delete the 'Start Packager' section from 'Build  phases' in the 'macos' target

![Delete Start Packager](doc/deleteStartPackager.png)

It should build now.
Now build *macOS*

![macOS](doc/buildmacOS.png)

## How this project was setup
Add dependencies to your project from:
- node_modules/react-native/Libraries
- node_modules/react-native/React

![Project dependencies](doc/dependencies.png)

Add link dependencies to your project

![Link dependencies](doc/linkDependencies.png)

Add project dependencies

![Frameworks](doc/addDependencies.png)