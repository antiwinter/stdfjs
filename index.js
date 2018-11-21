const REC = require('./rec')

const debug = (...args) => {
  // console.log(...args)
}

let tt = {
  poke(s) {
    if (!tt[s]) tt[s] = { v: 0, c: 0 }
    tt[s].a = new Date()
  },
  peek(s) {
    if (!tt[s]) return
    let n = new Date()
    tt[s].v += n - tt[s].a
    tt[s].c++
    tt[s].a = n
  }
}

module.exports = {
  parser() {
    let p = {
      buf: new Uint8Array(),
      cache: {},
      hook: { rec() {}, part() {}, end() {} },
      on(type, func) {
        p.hook[type] = func
        return p
      },

      concat(buf) {
        let _b = new Uint8Array(buf)
        let tmp = new Uint8Array(p.buf.length + _b.length)
        tmp.set(p.buf)
        tmp.set(_b, p.buf.length)

        return tmp
      },

      push(buf) {
        if (!buf) {
          // console.error(tt)

          return p.hook.end()
        } else {
          tt.poke('concat')

          p.buf = p.concat(buf)
          tt.peek('concat')

          for (;;) {
            tt.poke('getrec')

            let r = p.getRecord()
            tt.peek('getrec')

            if (r) {
              // emit record
              p.hook.rec(r)

              // process PRR
              switch (r.REC_TYP) {
                case 'PIR':
                  if (!p.cache[r.HEAD_NUM]) p.cache[r.HEAD_NUM] = {}
                  p.cache[r.HEAD_NUM][r.SITE_NUM] = { Site: r.SITE_NUM }
                  break

                case 'PTR':
                  let field = r.TEST_TXT
                  p.cache[r.HEAD_NUM][r.SITE_NUM][field] = r.RESULT
                  break

                case 'PRR':
                  p.cache[r.HEAD_NUM][r.SITE_NUM]['Bin'] = r.HARD_BIN
                  p.cache[r.HEAD_NUM][r.SITE_NUM]['X_COORD'] = r.X_COORD
                  p.cache[r.HEAD_NUM][r.SITE_NUM]['Y_COORD'] = r.Y_COORD
                  p.cache[r.HEAD_NUM][r.SITE_NUM]['PART_ID'] = r.Y_COORD

                  // console.log(
                  //   r.HEAD_NUM,
                  //   r.SITE_NUM,
                  //   Object.keys(p.cache[r.HEAD_NUM][r.SITE_NUM]).length
                  // )
                  // emit part
                  p.hook.part(p.cache[r.HEAD_NUM][r.SITE_NUM])
              }
            } else break
          }
        }
      },

      getRecord() {
        if (p.buf.length < 2) return
        let v = new DataView(p.buf.buffer)

        let len = v.getUint16(0, true)
        if (p.buf.length < len + 4) return

        let type = v.getUint8(2)
        let sub = v.getUint8(3)

        tt.poke('slice')

        p.buf = p.buf.slice(len + 4)
        tt.peek('slice')

        debug('len, type, sub', len, type, sub)

        if (!REC[type] || !REC[type][sub])
          return { REC_TYPE: 'unknown', BYTES: len }

        let t = REC[type][sub]
        let rec = { REC_TYP: t.REC_TYP }
        let pos = 4
        tt.poke('byteloop')

        for (let k in t.data) {
          let fmt = t.data[k]
          debug(
            'reading',
            fmt,
            '@',
            pos,
            len,
            new Uint8Array(v.buffer.slice(pos, pos + 10))
          )

          if (pos >= len + 4) break
          let _l
          switch (fmt) {
            case '1U':
            case '1B':
              rec[k] = v.getUint8(pos++)
              break
            case '1I':
              rec[k] = v.getInt8(pos++)
              break
            case '1C':
              rec[k] = String.fromCharCode(v.getUint8(pos++))
              break
            case '2U':
              rec[k] = v.getUint16(pos, true)
              pos += 2
              break
            case '2I':
              rec[k] = v.getInt16(pos, true)
              pos += 2
              break
            case '4U':
              rec[k] = v.getUint32(pos, true)
              pos += 4
              break
            case '4R':
              rec[k] = v.getFloat32(pos, true)
              pos += 4
              break
            case '?B':
              _l = v.getUint8(pos++)
              if (_l) {
                rec[k] = d.slice(pos, pos + _l)
                pos += _l
              } else rec[k] = 0

              break
            case '?C':
              _l = v.getUint8(pos++)
              if (_l) {
                rec[k] = String.fromCharCode.apply(
                  null,
                  new Uint8Array(v.buffer.slice(pos, pos + _l))
                )

                pos += _l
              } else rec[k] = ''

              break
            default:
              console.log('cannot parse format', fmt)
              throw 'unknown format'
          }
        }

        tt.peek('byteloop')

        return rec
      }
    }

    return p
  }
}
