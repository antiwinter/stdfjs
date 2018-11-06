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
const fs = require('fs')
const stdf = require('stdfjs')

let rs = fs.createReadStream('./1.stdf')

stdf
  .parser(rs)
  .on('rec', r => {
    console.log('%j', r)
  })
  // .on('part', p => {
  //   console.log(p)
  // })
```
