const tsconfigPaths = require('tsconfig-paths');

const tsconfig = require('../tsconfig.json');

const baseUrl = process.env.AVA_DEBUG
  ? tsconfig.compilerOptions.baseUrl
  : tsconfig.compilerOptions.outDir;

tsconfigPaths.register({
  baseUrl,
  paths: tsconfig.compilerOptions.paths || {},
});
