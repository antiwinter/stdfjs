const stdf = require('../')
const got = require('got')
const bz2 = require('unbzip2-stream')
const fs = require('fs')

let rs
if (!process.argv[2]) {
  // get the demo stdf file from internet
  let stdfbz2 = got.stream(
    'https://sourceforge.net/p/freestdf/libstdf/ci/master/tree/tests/stdf.bz2?format=raw'
  )

  // the demo file is in bzip2 format, so pipe it to decompress
  rs = stdfbz2.pipe(bz2())
} else {
  rs = fs.createReadStream(process.argv[2])
}

// feed the raw stdf stream to stdf parser
let parser = stdf
  .parser()
  .on('rec', r => {
    console.log('%j', r)
  })
  .on('part', p => {
    console.log('%j', p)
  })

rs.on('data', ck => {
  parser.push(ck)
}).on('end', () => {
  parser.push()
})
