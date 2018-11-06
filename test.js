const fs = require('fs')
const stdf = require('./')

let rs = fs.createReadStream('./1.stdf')

stdf.parser(rs).on('data', rec => {
  console.log('%j', rec)
})
