This is a package for parsing [stdf](https://en.wikipedia.org/wiki/Standard_Test_Data_Format) files. It is written in pure javascript. It is currently in its alpha state, which means it may have bugs, may be unstable. But at least you now have a package to process stdf file.

### Usage

**to Install**
```bash
npm i stdfjs
```

**to Use**
```js
const fs = require('fs')
const stdf = require('stdf')

let rs = fs.createReadStream('./1.stdf')

stdf.parser(rs).on('data', rec => {
  console.log('%j', rec)
})
```