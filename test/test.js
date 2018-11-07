const stdf = require('../')
const got = require('got')
const bz2 = require('unbzip2-stream')

// get the demo stdf file from internet
let stdfbz2 = got.stream(
  'https://sourceforge.net/p/freestdf/libstdf/ci/master/tree/tests/stdf.bz2?format=raw'
)

// the demo file is in bzip2 format, so pipe it to decompress
let rs = stdfbz2.pipe(bz2())

// feed the raw stdf stream to stdf parser
stdf
  .parser(rs)
  .on('rec', r => {
    console.log('%j', r)
  })
  // .on('part', p => {
  //   console.log(p)
  // })
