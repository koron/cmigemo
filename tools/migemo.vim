" vim:set ts=8 sts=2 sw=2 tw=0:
"
" migemo.vim
"   Direct search for Japanese with Romaji --- Migemo support script.
"
" Maintainer:  MURAOKA Taro <koron@tka.att.ne.jp>
" Last Change: 16-May-2002.

" Japanese Description:

if exists('plugin_migemo_disable')
  finish
endif

function! s:SearchDict()
  let path = $VIM . ',' . &runtimepath
  let dict = globpath(path, "dict/migemo-dict")
  if dict == ''
    let dict = globpath(path, "migemo-dict")
    if dict == ''
      return ''
    endif
  endif
  return matchstr(dict, "^[^\<NL>]*")
endfunction

if has('migemo')
  if &migemodict == '' || !filereadable(&migemodict)
    let &migemodict = s:SearchDict()
  endif
else
  " TODO: include megemo.vim by mattn
endif
