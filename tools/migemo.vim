" vi:set ts=8 sts=2 sw=2 tw=0:
"
" migemo.vim - Direct search for Japanese with Romaji
"		--- Migemo support script.
"
" Maintainer:  Muraoka Taro  <koron@tka.att.ne.jp>
" Last Change: 08-Sep-2001.
"
" Japanese Description:

if !has('migemo') || exists('plugin_migemo_disable')
  finish
endif

let s:migemosearchdir = $VIM . ',' . &runtimepath
if &migemodict == '' || !filereadable(&migemodict)
  let &migemodict = globpath(s:migemosearchdir, "dict/migemo-dict")
endif
if &migemodict == ''
  let &migemodict = globpath(s:migemosearchdir, "migemo-dict")
endif
