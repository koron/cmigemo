C/Migemo���C�u����������
                                                            Since: 15-Aug-2001
                                                                Version: 1.2.0
                                                  Author: MURAOKA Taro (KoRoN)
                                                     Last Change: 02-Jan-2013.

����
  C/Migemo��Migemo(Ruby/Migemo)��C����Ŏ����������̂ł��BC/Migemo���C�u������
  ���p����\�t�g�E�F�A�́u���[�}���̂܂ܓ��{���(�C���N�������^����)��������v
  �@�\�������Ƃ��\�ɂȂ�܂��BC����Ŏ����������Ƃɂ��A�{��Ruby/Migemo��
  ��ׁAC����ŏ����ꂽ�����̃\�t�g�E�F�A����̗��p���e�ՂɂȂ邱�ƁA�y��(����
  ��)���s���x�̌��オ���҂ł��܂��B

  �܂�C/Migemo�𗘗p���邽�߂ɂ͕ʓr�����t�@�C��(dict/migemo-dict)���쐬������
  �͓��肷��K�v������܂��B�����ł܂������̃[�����玫�����쐬���邱�Ƃ��ł���
  �����ARuby/Migemo��migemo-dict�𗬗p������ASKK�̎�������R���o�[�g���邱��
  ���\�ł��B�ڂ����͉��L�́u�����ɂ��āv�̃Z�N�V�������Q�Ƃ��Ă��������B

  Ruby/Migemo�͍��� �N���񂪍l�Ă����u���[�}�����͂���{�ꌟ���̂��߂̐��K�\��
  �ɕϊ�����v�c�[���ł��BEmacs��Ń��[�}���̂܂ܓ��{����C���N�������^������
  ���邽�߂ɁARuby��Emacs LISP�Ŏ�������Ă��܂����B

  - �{��Ruby/Migemo�T�C�g (Migemo�Ɋւ���ڍ׏��)
      http://migemo.namazu.org/
  - C/Migemo����E�z�z�T�C�g (C/Migemo�̏��)
      http://www.kaoriya.net/

�t�@�C���\��
  C/Migemo�̃p�b�P�[�W�͌��݂̂Ƃ��뎟�̂悤�ȃt�@�C���E�f�B���N�g������\����
  ��Ă��܂��B
  (�f�B���N�g��)
    compile/            :�e�v���b�g�z�[���p�̃��C�N�t�@�C���u����
    compile/vs6         :VS6�p�v���W�F�N�g
    compile/vs2003      :VS2003�p�v���W�F�N�g
    doc/                :�h�L�������g�u����
    dict/               :�����u����
    tools/              :�e��c�[��
    src/                :�\�[�X�t�@�C��
  (�t�@�C��)
    Makefile            :�������C�N�t�@�C��
    README.txt          :�e�h�L�������g�ւ̃|�C���^
    VERSION             :�o�[�W�����ԍ�
    config.mk           :�f�t�H���g�R���t�B�M�����[�V����
  (�\�[�X�R�[�h) src/��
    depend.mak          :�\���t�@�C���ƈˑ��֌W
    migemo.h            :���C�u�����𗘗p����̂��߂̃C���N���[�h�t�@�C��
    main.c              :���C�u�����𗘗p����T���v���v���O����(cmigemo)
    *.c                 :���C�u�����̃\�[�X
    *.h                 :���C�u�����̃\�[�X�p�C���N���[�h�t�@�C��

�R���p�C�����@
  �R���p�C���ɂ͈ȉ���3�i�K�̎菇������܂��B
    1. ���C�u�����ƃv���O�����̃r���h
    2. �����t�@�C���̃r���h
    3. �C���X�g�[��
  �u�����t�@�C���̃r���h�v�ɂ͕ʓr�ȉ���4�킪�K�v�ɂȂ�܂��B
    1. Perl
    2. �C���^�[�l�b�g�A�N�Z�X�v���O����(cURL, wget, fetch�̓��ǂꂩ1��)
    3. �𓀐L���v���O����gzip
    4. �G���R�[�h�ϊ��v���O����(nkf, qkc)
  �������܂�C/Migemo�̃r���h�ɕK�v�ȊO���v���O������config.mk��ҏW���邱��
  �ŕύX�\�ł��B�܂��ȉ��̃v���b�g�z�[���ʂ̃r���h���@�ŗ��p���Ă���悤��
  UNIX���C�N�Ȋ��ł͎����I��config.mk���C������d�g��(configure)�𗘗p�ł���
  ���Bconfig.mk�̃I���W�i���t�@�C����compile/config_default.mk�Ƃ��Ď��^���Ă�
  ��̂ł��ł������\�ł��B

  �e�v���b�g�z�[���ł̃r���h���@�̋�̗�͈ȉ����Q�Ƃ��Ă��������B
    1. Windows + VisualC++
    2. Windows + Cygwin
    3. Windows + Borland C++
    4. MacOS X + Developer Tools
    5. GNU/gcc:Linux��
  ���݂̂Ƃ���ȏ�5���œ���̊m�F�����Ă��܂��B5.��GNU/gcc�ɂ��Ă�FreeBSD
  5.0�ł��B

  (Windows + VisualC++)
  ���̃R�}���h��Release/����migemo.dll��cmigemo.exe���쐬����܂��B
    > nmake msvc
  �K�v�ȊO���v���O�����A�l�b�g���[�N�ڑ��������Ă����
    > nmake msvc-dict
  �Ŏ����t�@�C�����r���h�ł��܂��Bmigemo.dsw��VC++6.0�ŊJ���A�r���h������@��
  ����܂��B�ȏオ�I������Ύ��̃R�}���h�Ńe�X�g�v���O���������삵�܂��B
    > .\build\cmigemo -d dict/migemo-dict

  (Windows + Cygwin)
  �K�v�ȊO���v���O�����A�l�b�g���[�N�ڑ��𑵂��Ĉȉ������s���邱�ƂŃe�X�g�v��
  �O����cmigemo�Ǝ����t�@�C�����r���h����܂�:
    $ ./configure
    $ make cyg
    $ make cyg-dict
  ���s��cp932(Shift-JIS)�ŗ��p����Ȃ��:
    $ ./build/cmigemo -d dict/migemo-dict
  euc-jp�ŗ��p����Ȃ��:
    $ ./build/cmigemo -d dict/euc-jp.d/migemo-dict
  �����s���܂��B�C���X�g�[���ƃA���C���X�g�[����root�����Ŏ��̃R�}���h�����s��
  �邱�Ƃōs�Ȃ��܂��B�C���X�g�[���ꏊ�ɂ��Ă�config.mk���Q�Ƃ��Ă��������B
    # make cyg-install
    # make cyg-uninstall

  (Windows + Borland C++)
  ���̃R�}���h��migemo.dll��cmigemo.exe���쐬����܂��B
    > make bc
  �K�v�ȊO���v���O�����A�l�b�g���[�N�ڑ��������Ă����
    > nmake bc-dict
  �Ŏ����t�@�C�����r���h�ł��܂��B�ȏオ�I������Ύ��̃R�}���h�Ńe�X�g�v���O��
  �������삵�܂��B
    > .\build\cmigemo -d dict/migemo-dict

  (MacOS X + Developer Tools)
  �K�v�ȊO���v���O�����A�l�b�g���[�N�ڑ��𑵂��Ĉȉ������s���邱�ƂŃe�X�g�v��
  �O����cmigemo�Ǝ����t�@�C�����r���h����܂�:
    % ./configure
    % make osx
    % make osx-dict
  ���s��cp932(Shift-JIS)�ŗ��p����Ȃ��:
    % ./build/cmigemo -d dict/migemo-dict
  euc-jp�ŗ��p����Ȃ��:
    % ./build/cmigemo -d dict/euc-jp.d/migemo-dict
  �����s���܂��B�C���X�g�[���ƃA���C���X�g�[����root�����Ŏ��̃R�}���h�����s��
  �邱�Ƃōs�Ȃ��܂��B�C���X�g�[���ꏊ�ɂ��Ă�config.mk���Q�Ƃ��Ă��������B
    # make osx-install
    # make osx-uninstall
  �C���X�g�[���ɂ�莫���t�@�C����
    /usr/local/share/migemo/{�G���R�[�h��}
  �ɒu����܂��B

  (GNU/gcc:Linux��)
  �K�v�ȊO���v���O�����A�l�b�g���[�N�ڑ��𑵂��Ĉȉ������s���邱�ƂŃe�X�g�v��
  �O����cmigemo�Ǝ����t�@�C�����r���h����܂�:
    $ ./configure
    $ make gcc
    $ make gcc-dict
  ���s��cp932(Shift-JIS)�ŗ��p����Ȃ��:
    $ ./build/cmigemo -d dict/migemo-dict
  euc-jp�ŗ��p����Ȃ��:
    $ ./build/cmigemo -d dict/euc-jp.d/migemo-dict
  �����s���܂��B�C���X�g�[���ƃA���C���X�g�[����root�����Ŏ��̃R�}���h�����s��
  �邱�Ƃōs�Ȃ��܂��B�C���X�g�[���ꏊ�ɂ��Ă�config.mk���Q�Ƃ��Ă��������B
    # make gcc-install
    # make gcc-uninstall
  �C���X�g�[���ɂ�莫���t�@�C����
    /usr/local/share/migemo/{�G���R�[�h��}
  �ɒu����܂��B

���p��������
  C/Migemo��MIT���C�Z���X�ƓƎ����C�Z���X�̃f���A�����C�Z���X�Ƃ��Ĕz�z�����
  ���܂��B���p�҂͓s���ɉ����āA���K�����郉�C�Z���X��I�����ė��p���Ă�����
  ���B

  MIT���C�Z���X�ɂ��Ă�doc/LICENSE_MIT.txt���Q�Ƃ��Ă��������B
  �Ǝ����C���Z���X�ɂ��Ă�doc/LICENSE_j.txt���Q�Ƃ��Ă��������B

�����̒��쌠�E���p��������
  C/Migemo�ŗ��p���鎫���̏������A���p���������y�ђ��쌠���͂��̎����̒�߂��
  ����ɏ]���܂��BC/Migemo��SKK������p����悤�ɏ����ݒ肳��Ă��܂����ASKK��
  ���̗��p����������C/Migemo�̂���Ƃ͖@�I�ɕʂł��邱�Ƃɒ��ӂ��Ă��������B

����E�A����
  C/Migemo�Ɋւ��鎿��E�v�]���͑���(���L�A�h���X�Q��)�܂ŘA�����Ă��������B�\
  �t�g�E�F�A����C/Migemo���g�p�������ꍇ�̖₢���킹���󂯕t���܂��B

�ӎ�
  Migemo�𔭈Ă���Ruby/Migemo���쐬����AC/Migemo�ɂ��Ă̑��k��ML�Őe�؂ɓ�
  ���Ă������������� �N����Ɋ��ӂ������܂��B�܂��A���̔z�z�p�b�P�[�W�ɂ͈ȉ�
  �̕��X�ɂ��h�L�������g��A�C�f�A���܂܂�Ă��܂��B���肪�Ƃ��������܂��B

  (�A���t�@�x�b�g��)
  - AIDA Shinra
    Cygwin��Linux�pMakefile�̊�b
  - FUJISHIMA Hiroshi <pooh@nature.tsukuba.ac.jp>
    Solaris�pMakefile�̒�
  - MATSUMOTO Yasuhiro
    Borland C++�pMakefile�̊�b
    migemo.vim��cmigemo�ɑΉ�
    VB�p�T���v��
  - SUNAOKA Norifumi
    Solaris�pMakefile�̕s���
  - TASAKA Mamoru
    MIT���C�Z���X�ł̃����[�X�ɋ���
  - gageas (https://twitter.com/gageas)
    Migemo.cs�̏C��&����


�t�^

�����ɂ��� {{{1
  C/Migemo�ł̓��[�}������{��֕ϊ�����̂Ɏ����t�@�C��dict/migemo-dict��K�v
  �Ƃ��܂��B�t�@�C��dict/migemo-dict�͂��̃A�[�J�C�u�Ɋ܂܂�Ă��܂��񂪁A����
  �t�@�C���̃r���h���ɃC���^�[�l�b�g��ʂ��Ď����I�Ƀ_�E�����[�h����d�g�݂ɂ�
  ���Ă��܂��B�܂���ςł���������0����쐬���邱�Ƃ��\�ł��B

  �����Ɋւ���c�[���A���̎g�p�@��dict/Makefile���Q�Ƃ��Ă��������B

  C/Migemo�ł�
    1. dict/migemo-dict�ȊO�ɂ��A
    2. ���[�}���𕽉����ɕϊ����邽�߂̃t�@�C��(dict/roma2hira.dat)��A
    3. ��������Љ����ɕϊ����邽�߂̃t�@�C��(dict/hira2kata.dat)��A
    4. ���p������S�p�����ɕϊ����邽�߂̃t�@�C��(dict/han2zen.dat)
    5. �S�p�����𔼊p�����ɕϊ����邽�߂̃t�@�C��(dict/zen2han.dat)
  ���g�p���Ă��܂��B�����̑S�Ẵt�@�C���͒P�Ƀf�[�^�e�[�u���Ƃ��ċ@�\���Ă�
  �邾���łȂ��A�V�X�e���̃G���R�[�h(�����R�[�h)�̈Ⴂ���z������������S���Ă�
  �܂��B�܂��ɋ�����4�t�@�C����Windows�Ŏg���ꍇ�ɂ�cp932�ɁAUNIX��Linux��
  �g���ꍇ�ɂ�euc-jp�ɕϊ�����K�v������̂ł��B�ϊ����K�v���Ɨ\�z�����ꍇ��
  �́A�����t�@�C���̃r���h�Ɠ����Ɏ����I�ɍs�Ȃ���悤�ɂȂ��Ă��܂��B

  - Migemo DLL�z�z�y�[�W (cp932��migemo-dict������\)
      http://www.kaoriya.net/
  - Ruby/Migemo (euc-jp��migemo-dict������\)
      http://migemo.namazu.org/
  - SKK Openlab (�ŐV��SKK-JISYO.L������\)
      http://openlab.ring.gr.jp/skk/index-j.html

�^���t�@�����X {{{1
  C/Migemo�ŗ��p�����^�ɂ��ďq�ׂ�B

- migemo*;
    Migemo�I�u�W�F�N�g�Bmigemo_open()�ō쐬����Amigemo_close()�Ŕj�������B

- int (*MIGEMO_PROC_CHAR2INT)(const unsigned char*, unsigned int*);
    �o�C�g��(unsigned char*)�𕶎��R�[�h(unsigned int)�ɕϊ�����v���V�[�W��
    �^�BShift-JIS��EUC-JP�ȊO�̃G���R�[�h�̕�����������Ƃ��A�������͓��ꕶ��
    �̏������s�������Ƃ��ɒ�`����B�߂�l�͕�����̂������������o�C�g���ŁA0
    ��Ԃ��΃f�t�H���g�̃v���V�[�W�������s�����B���̎d�g�݂ŕK�v�ȕ���������
    �������{�����Ƃ��o����B

- int (*MIGEMO_PROC_INT2CHAR)(unsigned int, unsigned char*);
    �R�[�h(unsigned int)���o�C�g��(unsigned char*)�ɕϊ�����v���V�[�W���^�B
    Shift-JIS��EUC-JP�ȊO�̃G���R�[�h������������Ƃ��A�������͓��ꕶ���̏���
    ���s�������Ƃ��ɒ�`����B�߂�l�͏o�͂��ꂽ������̃o�C�g���ŁA0��Ԃ���
    �f�t�H���g�̃v���V�[�W�������s�����B���̎d�g�݂ŕK�v�ȕ��������ɏ������{
    �����Ƃ��o����B

�֐����t�@�����X {{{1
  C/Migemo���C�u�����Œ񋟂����API���ȉ��ŉ������B���ۂ̎g�p��̓A�[�J�C�u
  �Ɋ܂܂��main.c���Q�Ƃ̂��ƁB

- migemo* migemo_open(const char* dict);
    Migemo�I�u�W�F�N�g���쐬����B�쐬�ɐ�������ƃI�u�W�F�N�g���߂�l�Ƃ��ĕ�
    ��A���s�����NULL���Ԃ�Bdict�Ŏw�肵���t�@�C����migemo-dict�����Ƃ��ăI
    �u�W�F�N�g�쐬���ɓǂݍ��܂��B�����Ɠ����f�B���N�g����:
      1. roma2hira.dat  (���[�}�����������ϊ��\)
      2. hira2kata.dat  (���������J�^�J�i�ϊ��\)
      3. han2zen.dat    (���p���S�p�ϊ��\)
      3. zen2han.dat    (�S�p�����p�ϊ��\)
    �Ƃ������O�̃t�@�C�������݂���΁A���݂������̂������ǂݍ��܂��Bdict��
    NULL���w�肵���ꍇ�ɂ́A�������܂߂Ă����Ȃ�t�@�C�����ǂݍ��܂�Ȃ��B�t�@
    �C���̓I�u�W�F�N�g�쐬��ɂ�migemo_load()�֐����g�p���邱�ƂŒǉ��ǂݍ���
    ���ł���B

- void migemo_close(migemo* object);
    Migemo�I�u�W�F�N�g��j�����A�g�p���Ă������\�[�X���������B

- unsigned char* migemo_query(migemo* object, const unsigned char* query);
    query�ŗ^����ꂽ������(���[�}��)����{�ꌟ���̂��߂̐��K�\���֕ϊ�����B
    �߂�l�͕ϊ����ꂽ���ʂ̕�����(���K�\��)�ŁA�g�p���migemo_release()�֐���
    �n�����Ƃŉ�����Ȃ���΂Ȃ�Ȃ��B

- void migemo_release(migemo* object, unsigned char* string);
    �g���I�����migemo_query()�֐��œ���ꂽ���K�\�����������B

- int migemo_load(migemo* obj, int dict_id, const char* dict_file);
    Migemo�I�u�W�F�N�g�Ɏ����A�܂��̓f�[�^�t�@�C����ǉ��ǂݍ��݂���B
    dict_file�͓ǂݍ��ރt�@�C�������w�肷��Bdict_id�͓ǂݍ��ގ����E�f�[�^�̎�
    �ނ��w�肷����̂ňȉ��̂����ǂꂩ����w�肷��:

      MIGEMO_DICTID_MIGEMO      mikgemo-dict����
      MIGEMO_DICTID_ROMA2HIRA   ���[�}�����������ϊ��\
      MIGEMO_DICTID_HIRA2KATA   ���������J�^�J�i�ϊ��\
      MIGEMO_DICTID_HAN2ZEN     ���p���S�p�ϊ��\
      MIGEMO_DICTID_ZEN2HAN     �S�p�����p�ϊ��\

    �߂�l�͎��ۂɓǂݍ��񂾎�ނ������A��L�̑��ɓǂݍ��݂Ɏ��s�������Ƃ�����
    ���̉����Ԃ邱�Ƃ�����B
      MIGEMO_DICTID_INVALID     

- int migemo_is_enable(migemo* obj);
    Migemo�I�u�W�F�N�g��migemo_dict���ǂݍ��߂Ă��邩���`�F�b�N����B�L����
    migemo_dict��ǂݍ��߂ē����ɕϊ��e�[�u�����\�z�ł��Ă����0�ȊO(TRUE)���A
    �\�z�ł��Ă��Ȃ��Ƃ��ɂ�0(FALSE)��Ԃ��B

- int migemo_set_operator(migemo* object, int index, const unsigned char* op);
    Migemo�I�u�W�F�N�g���������鐳�K�\���Ɏg�p���郁�^����(���Z�q)���w�肷��B
    index�łǂ̃��^���������w�肵�Aop�Œu��������Bindex�ɂ͈ȉ��̒l���w��\
    �ł���:

      MIGEMO_OPINDEX_OR
        �_���a�B�f�t�H���g�� "|" �Bvim�ŗ��p����ۂ� "\|" �B
      MIGEMO_OPINDEX_NEST_IN
        �O���[�s���O�ɗp����J�����ʁB�f�t�H���g�� "(" �Bvim�ł̓��W�X�^\1�`\9
        �ɋL�������Ȃ��悤�ɂ��邽�߂� "\%(" ��p����BPerl�ł����l�̂��Ƃ��
        �_�ނȂ�� "(?:" ���g�p�\�B
      MIGEMO_OPINDEX_NEST_OUT
        �O���[�s���O�̏I����\�������ʁB�f�t�H���g�ł� ")" �Bvim�ł� "\)" �B
      MIGEMO_OPINDEX_SELECT_IN
        �I���̊J�n��\���J���p���ʁB�f�t�H���g�ł� "[" �B
      MIGEMO_OPINDEX_SELECT_OUT
        �I���̏I����\�����p���ʁB�f�t�H���g�ł� "]" �B
      MIGEMO_OPINDEX_NEWLINE
        �e�����̊Ԃɑ}�������u0�ȏ�̋󔒂������͉��s�Ƀ}�b�`����v�p�^�[
        ���B�f�t�H���g�ł� "" �ł���ݒ肳��Ȃ��Bvim�ł� "\_s*" ���w�肷��B

    �f�t�H���g�̃��^�����͓��ɒf�肪�Ȃ�����Perl�̂���Ɠ����Ӗ��ł���B�ݒ��
    ��������Ɩ߂�l��1(0�ȊO)�ƂȂ�A���s�����0�ɂȂ�B

- const unsigned char* migemo_get_operator(migemo* object, int index);
    Migemo�I�u�W�F�N�g���������鐳�K�\���Ɏg�p���Ă��郁�^����(���Z�q)���擾��
    ��Bindex�ɂ��Ă�migemo_set_operator()�֐����Q�ƁB�߂�l�ɂ�index�̎w��
    ����������΃��^�������i�[����������ւ̃|�C���^���A�s���ł����NULL����
    ��B

- void migemo_setproc_char2int(migemo* object, MIGEMO_PROC_CHAR2INT proc);
    Migemo�I�u�W�F�N�g�ɃR�[�h�ϊ��p�̃v���V�[�W����ݒ肷��B�v���V�[�W���ɂ�
    ���Ă̏ڍׂ́u�^���t�@�����X�v�Z�N�V������MIGEMO_PROC_CHAR2INT���Q�ƁB

- void migemo_setproc_int2char(migemo* object, MIGEMO_PROC_INT2CHAR proc);
    Migemo�I�u�W�F�N�g�ɃR�[�h�ϊ��p�̃v���V�[�W����ݒ肷��B�v���V�[�W���ɂ�
    ���Ă̏ڍׂ́u�^���t�@�����X�v�Z�N�V������MIGEMO_PROC_INT2CHAR���Q�ƁB

- void migemo_setproc_int2char_escape_meta_characters(migemo* object, MIGEMO_PROC_INT2CHAR proc);
    Migemo�I�u�W�F�N�g�ɃR�[�h�ϊ��p�̃v���V�[�W����ݒ肷��B�v���V�[�W����
    ���Ă̏ڍׂ́u�^���t�@�����X�v�Z�N�V������MIGEMO_PROC_INT2CHAR���Q�ƁB
    ���̃v���V�[�W���ł͐��K�\���̃��^�L�����N�^���G�X�P�[�v����B

�R�[�f�B���O�T���v�� {{{1
  C/Migemo�𗘗p�����R�[�f�B���O��������B�ȉ��̃T���v���͈�؂̃G���[�������s
  �Ȃ��Ă��Ȃ��̂ŁA���ۂ̗��p���ɂ͒��ӂ��K�v�B
    #include <stdio.h>
    #include "migemo.h"
    int main(int argc, char** argv)
    {
        migemo *m;
        /* C/Migemo�̏���: �����Ǎ��ɂ̓G���[���o�̂���load�𐄏� */
        m = migemo_open(NULL);
        migemo_load(m, MIGEMO_DICTID_MIGEMO, "./dict/migemo-dict");
        /* �K�v�ȉ񐔂���query���s�Ȃ� */
        {
            unsigned char* p;
            p = migemo_query(m, "nezu");
            printf("C/Migemo: %s\n", p);
            migemo_release(m, p); /* query�̌��ʂ͕K��release���� */
        }
        /* ���p���I�����migemo�I�u�W�F�N�g��close���� */
        migemo_close(m);
        return 0;
    }

�X�V�ӏ� {{{1
  �� (1.3 �J����)
    VC9(VisualStudio2008)�ł�64bit�ł̃R���p�C���ɑΉ�
    VC9(VisualStudio2008)�ł̃R���p�C���ɑΉ�
    ���[�}�����q���ŏI���ۂɁuxtu{�q��}{�ꉹ}�v��������悤�ɕύX
    configure��--prefix���@�\���Ă��Ȃ��������̏C��
    ��query�ł̎���������ignore case��
    �p�P��̃G���g�����������Ő��K��
    VB�p�T���v��(tools/clsMigemo.cls)��ǉ�
    �p�^�[���̐�����������(rxget.c:rxgen_add)
    VS2003�p�v���W�F�N�g�t�@�C���ǉ�
    �S�p�����p�ϊ���ǉ�
    �S�p�����p������ǉ�
    ���[�}���ϊ��Ń}���`�o�C�g�������l��(Vim�f����:1281)
    CP932/EUCJP/UTF8�̃G���R�[�h�������ʋ@�\��ǉ�
    migemo.vim��cmigemo.exe�Ή��̉��ǔłɍ����ւ�
    �w���v���b�Z�[�W���̗]���ȋ󔒂��폜
  �� 29-Dec-2003 (1.2 ������)
    ���p�啶����S�p�啶���ɕϊ��ł��Ȃ��o�O���C��
    �����[�X�p�Ƀh�L�������g���C��
    configure�X�N���v�g�𓱓�
    Doxygen�𓱓�����
    query���̂��̂Ŏ����������̂�Y��Ă����̂��C��
    tools/Migemo.cs:�ُ̍C��
    �r���h�f�B���N�g����ύX(����m�F��:mvc,cyg,gcc,bc5)
    (1.1.023)migemo.c:EXPORTS�̍폜��
    (1.1.022)dict/roma2hira.dat��~���`�̕ϊ���ǉ�
    (1.1.021)Windows�pMakefile���C��
    (1.1.020)romaji.c�̃e�X�g�R�[�h�𕪗�
    (1.1.019)�����ǂݍ��ݎ��̃G���[��s���S�Ȃ��猵����
    (1.1.018)roma2hira.dat�Ƀw�{������m[bmp]�y��tch[aiueo]��ǉ�
    (1.1.017)���p���������𐮗�
    (1.1.016)cmigemo���T�u����(���[�U����)�ɑΉ�
    (1.1.015)cmigemo�o�͌��fflush()��ǉ�
    (1.1.014)MSVC�p�v���W�F�N�g�t�@�C����msvc/�ȉ��ֈړ�
    (1.1.013)cmigemo��--emacs/-e�y��--nonewline/-n��ǉ�
    (1.1.012)mkpkg������
    (1.1.011)dict/roma2hira.dat��[bp]y[aiueo]�̃G���g����ǉ�
    (1.1.010)migemo.vim��cmigemo�ɑΉ�(by.�܂��񂳂�)
    (1.1.009)Solaris�ŃR���p�C���ł���悤�ɂ��邽�߂̕ύX
    (1.1.008)SKK�����̔z�z�`�ԕύX�ɒǏ]
    (1.1.007)�f�o�b�O���b�Z�[�W���R�����g�ɂ��폜
    (1.1.006)config.mk�ɐݒ����R�����g�Ƃ��Ēǉ�
    (1.1.005)C#�p�T���v��tools/*.cs��ǉ�
    (1.1.004)�A���߂ɑΉ�(���ň���)
    (1.1.003)Solaris�p���C�N�t�@�C��Make_sun.mak��ǉ��A������`�F�b�N
    (1.1.002)������1�s�ɕ����G���g�����o�͂ł��Ȃ��Ȃ��Ă��������C��
    (1.1.001)migemo.vim��UNIX�n�ɑΉ�
  �� 27-May-2002 (1.1 ������)
    �����Ń����[�X
    �h�L�������g�u���b�V���A�b�v
    Makefile�C��:release��./Release�̂��߂Ƀr���h�ł��Ȃ��������
  �� 16-May-2002 (1.1-beta2)
    BC5�Ή��̂��߃u�`�؂ꐡ�O�c(_ _;;;
    Makefile�̍\���ύX(BC5�Ή��̂���)
    �ӎ��ǉ�
    �p�b�P�[�W�쐬�p�X�N���v�g�̒ǉ�
    cmigemo���v���O��������g���₷���B
  �� 15-May-2002 (1.1-beta1)
    migemo_set_operator()�̖߂�l�̈Ӗ���ύX
    �h�L�������g�u���b�V���A�b�v
    Cygwin/MacOS X/Linux�p��Makefile���쐬
    strip.pl��lensort.pl��tool/optimize-dict.pl�ɓ���
    tool/conv.pl��tool/skk2migemodict.pl�ɖ��̕ύX
    �h�L�������g�C��
    cache���œK�����č����� (mnode_load())
    wordbuf_add()���œK�����č����� (wordbuf.c)
    migemo�����ǂݍ��ݎ��s�����o�ł��Ȃ��Ȃ��Ă����o�O�C��
    mnode���܂Ƃ߂Ċm�ۂ��邱�Ƃō����� (mnode.c��)
    �����𒷂��~���Ƀ\�[�g (tools/lensort.pl)
    �N����1������2�������� (mnode.c)
  �� 21-Aug-2001
    main.c��gets()��fgets()�ɕύX
}}}

-------------------------------------------------------------------------------
                  �����鎖�ւ̋����ӎu�������Ɏ����ƈقȂ鐶���������ԐS�ƂȂ�
                                    MURAOKA Taro/�������Y<koron@tka.att.ne.jp>

 vi:set ts=8 sts=2 sw=2 tw=78 et fdm=marker ft=memo:
