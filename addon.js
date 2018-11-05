var addon = require('bindings')('addon.node')

addon.parse('./1.stdf', d => {
  console.log('back:', d)
})
