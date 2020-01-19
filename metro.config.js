/**
 * Metro configuration for React Native
 * https://github.com/facebook/react-native
 *
 * @format
 */
if (require('os').platform() === 'win32') {
  const fs = require('fs');
  const path = require('path');
  const blacklist = require('metro-config/src/defaults/blacklist');

  const rnPath = fs.realpathSync(
    path.resolve(require.resolve('react-native/package.json'), '..'),
  );
  const rnwPath = fs.realpathSync(
    path.resolve(require.resolve('react-native-windows/package.json'), '..'),
  );

  module.exports = {
    resolver: {
      extraNodeModules: {
        // Redirect react-native to react-native-windows
        'react-native': rnwPath,
        'react-native-windows': rnwPath,
      },
      // Include the macos platform in addition to the defaults because the fork includes macos, but doesn't declare it
      platforms: ['windows', 'macos'],
      providesModuleNodeModules: ['react-native-windows'],
      // Since there are multiple copies of react-native, we need to ensure that metro only sees one of them
      // This should go in RN 0.61 when haste is removed
      blacklistRE: blacklist([
        new RegExp(
          `${(path.resolve(rnPath) + path.sep).replace(/[/\\\\]/g, '[/\\\\]')}.*`,
        ),

        // This stops "react-native run-windows" from causing the metro server to crash if its already running
        new RegExp(
          `${path
            .resolve(__dirname, 'windows')
            .replace(/[/\\\\]/g, '[/\\\\]')}.*`,
        ),
      ]),
    },
    transformer: {
      getTransformOptions: async () => ({
        transform: {
          experimentalImportSupport: false,
          inlineRequires: false,
        },
      }),
    },
  };

}
else {
  module.exports = {
    resolver: {
      platforms: ['win32', 'macos'],
    }
  };
}
