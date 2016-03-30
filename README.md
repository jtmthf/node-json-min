# node-json-min
C++ add-on of json minification

## Install via NPM
```
npm install node-json-min
```
## Usage
```javascript
var minimizer = require('node-json-min');
minimizer.minify(' "key" : "value" ', function(result) {
  // result == '"key":"value"'
});
```

## Test
```
npm test
```
