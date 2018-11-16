Parses [stdf](https://en.wikipedia.org/wiki/Standard_Test_Data_Format) files into records or parts.

It is written in pure javascript.

It is currently in its alpha state, which means it may have bugs, may be unstable.

But at least you now have a package to process stdf file.

### Usage

**to Install**

```bash
npm i stdfjs --save
```

**to Use**

```js
const stdf = require('stdfjs')
const got = require('got')
const bz2 = require('unbzip2-stream')
const fs = require('fs')

// get the demo stdf file from internet
let stdfbz2 = got.stream(
  'https://sourceforge.net/p/freestdf/libstdf/ci/master/tree/tests/stdf.bz2?format=raw'
)

// the demo file is in bzip2 format, so pipe it to decompress
let rs = stdfbz2.pipe(bz2())

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
```

### Note

Because the new implementation uses [DataView]() instead of node [Buffer](), the performance is about 3 times slower than version `1.0.x` (commit ID `6083137cdf6a4f5f3c876e634e6c797ed44c810a`).

But this change enables the availability of this package in browser.
