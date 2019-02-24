// base for debug and proper config
const configBase = {
  cache: true,
  failFast: false,
  verbose: true,
};

// config for running tests with magic assert enabled
// this allows for better error reports
// but files need to be transpiled after each change
const configProper = {
  files: [
    'transpiled/spec/**/*.spec.js',
  ],
};

// config for debugging tests
// this allows for easy debugging
// but magic assert has to be disabled
const configDebug = {
  compileEnhancements: false,
  extensions: [
    'ts',
  ],
  files: [
    'src/spec/**/*.spec.ts',
  ],
  require: [
    'ts-node/register',
  ],
};

const configExtension = process.env.AVA_DEBUG
  ? configDebug
  : configProper;

export default Object.assign(configBase, configExtension);
