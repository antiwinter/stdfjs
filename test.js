const fs = require('fs')
const parser = require('./index').parser(fs.createReadStream('./1.stdf'))

parser.on('data', d => {
  console.log('%j', d)
})
