const addon = require('./build/Release/nedis');

var client = new addon.Nedis('something');
const rv = client.Greet('Stranger');
