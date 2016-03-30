var expect    = require('chai').expect;
var fs        = require('fs');
var minify    = require('node-json-minify');
var minimizer = require('..');

describe('Parts of JSON', function() {
  it('handles empty strings', function(done) {
    minimizer.minify('', function(result) {
      expect(result).to.equal('');
      done();
    });
  });

  it('handles single line comments', function(done) {
    minimizer.minify('//random crap', function(result) {
      expect(result).to.equal('');
      done();
    });
  });

  it('handles block comments', function(done) {
    minimizer.minify('/* crap \n more crap \n and */', function(result) {
      expect(result).to.equal('');
      done();
    });
  });

  it('handles key value', function(done) {
    minimizer.minify('"key" : "value"', function(result) {
      expect(result).to.equal('"key":"value"');
      done();
    });
  });
});

describe('File handling', function() {
  var smallInput     = fs.readFileSync(__dirname + '/input/small.json', 'utf-8');
  var smallExpected  = minify(smallInput);
  var mediumInput    = fs.readFileSync(__dirname + '/input/medium.json', 'utf-8');
  var mediumExpected = minify(mediumInput);
  var largeInput     = fs.readFileSync(__dirname + '/input/large.json', 'utf-8');
  var largeExpected  = minify(largeInput);
  var hugeInput     = fs.readFileSync(__dirname + '/input/huge.json', 'utf-8');
  var hugeExpected  = minify(hugeInput);

  it('handles small files', function(done) {
    minimizer.minify(smallInput, function(result) {
      expect(result).to.equal(smallExpected);
      done();
    });
  });

  it('handles medium files', function(done) {
    minimizer.minify(mediumInput, function(result) {
      expect(result).to.equal(mediumExpected);
      done();
    });
  });

  it('handles large files', function(done) {
    minimizer.minify(largeInput, function(result) {
      expect(result).to.equal(largeExpected);
      done();
    });
  });

  it('handles huge files', function(done) {
    minimizer.minify(hugeInput, function(result) {
      expect(result).to.equal(hugeExpected);
      done();
    });
  });
});
