" vim:set ts=8 sts=2 sw=2 tw=0:
"
" migemo.vim
"   Direct search for Japanese with Romaji --- Migemo support script.
"
" Maintainer:  MURAOKA Taro <koron@tka.att.ne.jp>
" Modified:    Yasuhiro Matsumoto <mattn_jp@hotmail.com>
" Last Change: 07-Jun-2003.

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
  " non-builtin version
  let g:migemodict = s:SearchDict()
  command! -nargs=* Migemo :call <SID>MigemoSearch(<q-args>)
  nnoremap <silent> <leader>mi :call <SID>MigemoSearch('')<cr>

  function! s:MigemoSearch(word)
    if executable('cmigemo') == ''
      echohl ErrorMsg
      echo 'Error: cmigemo is not installed'
      echohl None
      return
    endif
  
    let retval = a:word != '' ? a:word : input('MIGEMO:')
    if retval == ''
      return
    endif
    let retval = system('cmigemo -v -w "'.retval.'" -d "'.g:migemodict.'"')
    if retval == ''
      return
    endif
    let @/ = retval
    let v:errmsg = ''
    silent! normal n
    if v:errmsg != ''
      echohl ErrorMsg
      echo v:errmsg
      echohl None
    endif
  endfunction
endif
