const fs = require('fs')
const stdf = require('../')

let rs = fs.createReadStream('../1.stdf')

stdf
  .parser(rs)
  // .on('rec', r => {
  //   console.log('%j', r)
  // })
  .on('part', p => {
    console.log(p)
  })
