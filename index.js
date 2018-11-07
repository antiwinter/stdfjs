const stream = require('stream')
const events = require('events')
const REC = require('./rec')

const debug = (...args) => {
  // console.log(...args)
}

module.exports = {
  parser(rs, opts) {
    let p = {
      opts: opts,
      cache: {},
      buf: Buffer.alloc(0),
      ws: stream.Writable(),
      ee: new events.EventEmitter(),

      update(buf, done) {
        if (!buf) {
          p.ee.emit('end')
          return
        } else {
          p.buf = Buffer.concat([p.buf, buf])
          for (;;) {
            let r = p.getRecord()
            if (r) {
              p.ee.emit('rec', r)

              // process PRR
              switch (r.REC_TYP) {
                case 'PIR':
                  if (!p.cache[r.HEAD_NUM]) p.cache[r.HEAD_NUM] = {}
                  p.cache[r.HEAD_NUM][r.SITE_NUM] = { ':::Site': r.SITE_NUM }
                  break

                case 'PTR':
                  let field = null
                  if (p.opts && p.opts.partConversion) {
                    field = p.opts.partConversion[r.TEST_TXT]
                    if (!field) break
                  } else {
                    field = r.TEST_TXT
                  }

                  p.cache[r.HEAD_NUM][r.SITE_NUM][field] = r.RESULT
                  break

                case 'PRR':
                  p.cache[r.HEAD_NUM][r.SITE_NUM][':::Bin'] = r.HARD_BIN
                  p.ee.emit('part', p.cache[r.HEAD_NUM][r.SITE_NUM])
                  break
              }
            } else break
          }
        }
        done()
      },

      getRecord() {
        if (p.buf.length < 2) return
        let len = p.buf.readUInt16LE(0)
        if (p.buf.length < len + 4) return

        let type = p.buf.readUInt8(2)
        let sub = p.buf.readUInt8(3)
        let d = p.buf.slice(4, len + 4)

        p.buf = p.buf.slice(len + 4)

        debug('len, type, sub', len, type, sub)

        if (!REC[type] || !REC[type][sub])
          return { REC_TYPE: 'unknown', BYTES: len }

        let t = REC[type][sub]
        let rec = { REC_TYP: t.REC_TYP }
        let pos = 0
        for (let k in t.data) {
          let fmt = t.data[k]
          debug('reading', fmt, '@', pos)
          if (pos >= d.length) break
          switch (fmt) {
            case '1U':
            case '1B':
              rec[k] = d.readUInt8(pos++)
              break
            case '1I':
              rec[k] = d.readInt8(pos++)
              break
            case '1C':
              rec[k] = String.fromCharCode(d.readUInt8(pos++))
              break
            case '2U':
              rec[k] = d.readUInt16LE(pos)
              pos += 2
              break
            case '2I':
              rec[k] = d.readInt16LE(pos)
              pos += 2
              break
            case '4U':
              rec[k] = d.readUInt32LE(pos)
              pos += 4
              break
            case '4R':
              rec[k] = d.readFloatLE(pos)
              pos += 4
              break
            case '?B':
              len = d.readUInt8(pos++)
              if (len) {
                rec[k] = d.slice(pos, pos + len)
                pos += len
              } else rec[k] = 0

              break
            case '?C':
              len = d.readUInt8(pos++)
              if (len) {
                rec[k] = d.toString('utf-8', pos, pos + len)
                pos += len
              } else rec[k] = ''

              break
            default:
              console.log('cannot parse format', fmt)
              throw 'unknown format'
          }
          debug('got', rec[k])
        }
        return rec
      }
    }

    p.ws._write = (chunk, enc, next) => {
      p.update(chunk, next)
    }

    p.ws.on('finish', () => {
      p.update()
    })

    rs.pipe(p.ws)
    return p.ee
  }
}
