addToLibrary({
  $ALLOC_NORMAL: 0,
  $allocate: function(slab, allocator) {
    var ptr = _malloc(slab.length);
    if (typeof HEAPU8 !== 'undefined') {
      HEAPU8.set(slab, ptr);
    } else if (typeof Module !== 'undefined' && Module.HEAPU8) {
      Module.HEAPU8.set(slab, ptr);
    }
    return ptr;
  }
});
