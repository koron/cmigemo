" vim:set ts=8 sts=2 sw=2 tw=0:
"
" migemo.vim
"   Direct search for Japanese with Romaji --- Migemo support script.
"
" Maintainer:  MURAOKA Taro <koron@tka.att.ne.jp>
" Last Change: 01-Jun-2002.

" Japanese Description:

if exists('plugin_migemo_disable')
  finish
endif

function! s:SearchDict()
  let path = $VIM . ',' . &runtimepath
  let dict = globpath(path, "dict/migemo-dict")
  if dict == ''
    let dict = globpath(path, "migemo-dict")
  endif
  if dict == ''
    let dict = '/usr/local/share/migemo/'.&encoding.'/migemo-dict'
    if !filereadable(dict)
      let dict = ''
    endif
  endif
  return matchstr(dict, "^[^\<NL>]*")
endfunction

if has('migemo')
  if &migemodict == '' || !filereadable(&migemodict)
    let &migemodict = s:SearchDict()
  endif

  " ƒeƒXƒg
  function! s:SearchChar(dir)
    let input = nr2char(getchar())
    let pat = migemo(input)
    call search('\%(\%#.\{-\}\)\@<='.pat)
    noh
  endfunction
  nnoremap <Leader>f :call <SID>SearchChar(0)<CR>
else
  " TODO: include megemo.vim by mattn
endif
