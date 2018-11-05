{
  "targets": [
    {
      "target_name": "addon",
      "sources": ["addon.cc", "lib/libstdf.c", "lib/dtc.c", "lib/rec.c"],
      "include_dirs": ["<!(node -e \"require('nan')\")", "lib", "lib/include"]
    }
  ]
}
