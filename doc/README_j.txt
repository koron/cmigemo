C/Migemoライブラリ説明書
                                                             since 15-Aug-2001
                                                                   Version 1.1
                                                  Author: MURAOKA Taro (KoRoN)
                                                     Last Change: 14-May-2002.

説明
  C/MigemoはMigemo(Ruby/Migemo)をC言語で実装したものです。C/Migemoライブラリを
  利用するソフトウェアは「ローマ字のまま日本語を(インクリメンタルに)検索する」
  機能を持つことが可能になります。C言語で実装したことにより、本家Ruby/Migemoに
  比べ、C言語で書かれた多くのソフトウェアからの利用が容易になること、及び(たぶ
  ん)実行速度の向上が期待できます。

  またC/Migemoを利用するためには別途辞書ファイル(dict/migemo-dict)を作成もしく
  は入手する必要があります。自分でまったくのゼロから辞書を作成することもできま
  すが、Ruby/Migemoのmigemo-dictを流用したり、SKKの辞書からコンバートすること
  も可能です。詳しくは下記の「辞書について」のセクションを参照してください。

  Ruby/Migemoは高林 哲さんが考案した「ローマ字入力を日本語検索のための正規表現
  に変換する」ツールです。Emacs上でローマ字のまま日本語をインクリメンタル検索
  するために、RubyとEmacs LISPで実装されていました。

  - 本家Ruby/Migemoサイト (Migemoに関する詳細情報)
      http://migemo.namazu.org/
  - C/Migemo製作・配布サイト (C/Migemoの情報)
      http://www.kaoriya.net/

ファイル構成
  C/Migemoのパッケージは現在のところ次のようなファイル・ディレクトリから構成さ
  れています。
    Makefile            :cygwin make + VC6によるテスト用メイクファイル
    Make_*.mak          :各プラットホーム用のメイクファイル
    migemo.dsw          :VC6のワークスペース
    *.dsp               :VC6用プロジェクト(migemo.dswから参照される)
    migemo.h            :ライブラリを利用するのためのインクルードファイル
    main.c              :ライブラリを利用するサンプルプログラム
    *.c                 :ライブラリのソース
    *.h                 :ライブラリのソース用インクルードファイル
    dict/               :辞書置き場
    tool/               :各種ツール
    testdata/           :開発テストに利用したデータの置き場

コンパイル方法
  現在はWindows上でVC++6を用いてDLLをコンパイルするためのファイルしか用意され
  ていません。DLLを作成するにはmigemo.dswをVisual Studioで開き、dll_migemoプロ
  ジェクトをビルドすれば完了です。

  コンパイル自体がcygwinやLinux上で可能なことは確認してあります。ソースのある
  ディレクトリで:
    $ gcc *.c -o migemo
  としてみてください。サンプルアプリケーションmigemoがコンパイルされます。
  Windows以外のプラットホーム(UNIXやLinux)で使用する場合には、辞書ファイルのエ
  ンコードをコンバートする必要が生じる可能性があります。辞書についての詳細は
  「辞書について」のセクションを参照してください。autotoolsによるライブラリと
  してコンパイルするための:
    $ ./configure && make && make install
  は準備していません。どなたかやっていただけませんか?。

利用条件
  このC/Migemoライブラリは以下の条件に同意できる方のみ利用が許可されます。

  1. ソースコードの著作権は放棄しません。
  2. このライブラリを使用したために生じた損害については一切補償しません。
  3. 詳細は決まっていませんが、C/Migemoのソースコードは幅広く低コストで利用で
     きるようにLGPLかBSD、もしくはそれらに準ずるライセンスを適用します。

  特に辞書のライセンスが非常にグレーなので確定できていないのです。

質問・連絡先
  C/Migemoに関する質問・要望等は村岡(下記アドレス参照)まで連絡してください。ソ
  フトウェアからC/Migemoを使用したい場合の問い合わせも受け付けます。

謝辞
  Migemoを発案されRuby/Migemoを作成され、C/Migemoについての相談にMLで親切に答
  えていただいた高林 哲さんに。


辞書について
  C/Migemoではローマ字を日本語へ変換するのに辞書ファイルdict/migemo-dictを必要
  とします。ファイルdict/migemo-dictはこのアーカイブに含まれていませんが、
    1. vim向けにコンパイル済みのMigemo DLLから持ってくるか
    2. Ruby/Migemoのものを流用するか
    3. SKKの辞書ファイルからコンバートする
  ことができます。

  SKKからコンバートするためのツールはPerlで書かれたtool/conv.plとして収録され
  ています。SKK-JISYO.Lを下記URLから取得して次のようにコンバートします。
    $ perl tool/conv.pl < SKK-JISYO.L > dict/migemo-dict
  tool/strip.plは辞書ファイルから重複単語を削除するためのツールです。

  C/Migemoでは
    1. dict/migemo-dict以外にも、
    2. ローマ字を平仮名に変換するためのファイル(dict/roma2hira.dat)や、
    3. 平仮名を片仮名に変換するためのファイル(dict/hira2kata.dat)や、
    4. 半角文字を全角文字に変換するためのファイル(dict/han2zen.dat)
  を使用しています。これらの全てのファイルは単にデータテーブルとして機能してい
  るだけでなく、システムのエンコード(漢字コード)の違いを吸収する役割も担ってい
  ます。つまり先に挙げた4ファイルをWindowsで使う場合にはcp932に、UNIXやLinuxで
  使う場合にはeuc-jpに変換する必要があるのです。変換にはnkfやqkcを使うと良いで
  しょう。

  - Migemo DLL配布ページ (cp932のmigemo-dictが入手可能)
      http://www.kaoriya.net/~koron/
  - Ruby/Migemo (euc-jpのmigemo-dictが入手可能)
      http://migemo.namazu.org/
  - SKK Openlab (最新のSKK-JISYO.Lが入手可能)
      http://openlab.ring.gr.jp/skk/index-j.html

型リファレンス
  C/Migemoで利用される型について述べる。

- migemo*;
    Migemoオブジェクト。migemo_open()で作成され、migemo_close()で破棄される。

- int (*MIGEMO_PROC_CHAR2INT)(unsigned char*, unsigned int*);
    文字列(unsigned char*)をコード(unsigned int)に変換するプロシージャ型。
    Shift-JISやEUC-JP以外のエンコードの文字列を扱うとき、もしくは特殊文字の処
    理を行いたいときに定義する。戻り値は文字列のうち処理したバイト数で、0を返
    せばデフォルトのプロシージャが実行される。この仕組みで必要な文字だけに処
    理を施すことが出来る。

- int (*MIGEMO_PROC_INT2CHAR)(unsigned int, unsigned char*);
    コード(unsigned int)を文字列(unsigned char*)に変換するプロシージャ型。
    Shift-JISやEUC-JP以外のエンコード文字列を扱うとき、もしくは特殊文字の処理
    を行いたいときに定義する。戻り値は出力された文字列のバイト数で、0を返せば
    デフォルトのプロシージャが実行される。この仕組みで必要な文字だけに処理を施
    すことが出来る。

関数リファレンス
  C/Migemoライブラリで提供されるAPIを以下で解説する。実際の使用例はアーカイブ
  に含まれるmain.cを参照のこと。

- migemo* migemo_open(char* dict);
    Migemoオブジェクトを作成する。作成に成功するとオブジェクトが戻り値として返
    り、失敗するとNULLが返る。dictで指定したファイルがmigemo-dict辞書としてオ
    ブジェクト作成時に読み込まれる。辞書と同じディレクトリに:
      1. roma2hira.dat  (ローマ字→平仮名変換表)
      2. hira2kata.dat  (平仮名→カタカナ変換表)
      3. han2zen.dat    (半角→全角変換表)
    という名前のファイルが存在すれば、存在したものだけが読み込まれる。dictに
    NULLを指定した場合には、辞書を含めていかなるファイルも読み込まれない。ファ
    イルはオブジェクト作成後にもmigemo_load()関数を使用することで追加読み込み
    ができる。

- void migemo_close(migemo* object);
    Migemoオブジェクトを破棄し、使用していたリソースを解放する。

- unsigned char* migemo_query(migemo* object, unsigned char* query);
    queryで与えられた文字列(ローマ字)を日本語検索のための正規表現へ変換する。
    戻り値は変換された結果の文字列(正規表現)で、使用後はmigemo_release()関数へ
    渡すことで解放しなければならない。

- void migemo_release(migemo* object, unsigned char* string);
    使い終わったmigemo_query()関数で得られた正規表現を解放する。

- int migemo_load(migemo* obj, int dict_id, char* dict_file);
    Migemoオブジェクトに辞書、またはデータファイルを追加読み込みする。
    dict_fileは読み込むファイル名を指定する。dict_idは読み込む辞書・データの種
    類を指定するもので以下のうちどれか一つを指定する:

      MIGEMO_DICTID_MIGEMO      mikgemo-dict辞書
      MIGEMO_DICTID_ROMA2HIRA   ローマ字→平仮名変換表
      MIGEMO_DICTID_HIRA2KATA   平仮名→カタカナ変換表
      MIGEMO_DICTID_HAN2ZEN     半角→全角変換表

    戻り値は実際に読み込んだ種類を示し、上記の他に読み込みに失敗したことを示す
    次の価が返ることがある。
      MIGEMO_DICTID_INVALID     

- int migemo_is_enable(migemo* obj);
    Migemoオブジェクトにmigemo_dictが読み込めているかをチェックする。有効な
    migemo_dictを読み込めて内部に変換テーブルが構築できていれば0以外(TRUE)を、
    構築できていないときには0(FALSE)を返す。

- int migemo_set_operator(migemo* object, int index, unsigned char* op);
    Migemoオブジェクトが生成する正規表現に使用するメタ文字(演算子)を指定する。
    indexでどのメタ文字かを指定し、opで置き換える。indexには以下の値が指定可能
    である:

      MIGEMO_OPINDEX_OR
        論理和。デフォルトは "|" 。vimで利用する際は "\|" 。
      MIGEMO_OPINDEX_NEST_IN
        グルーピングに用いる開き括弧。デフォルトは "(" 。vimではレジスタ\1～\9
        に記憶させないようにするために "\%(" を用いる。Perlでも同様のことを目
        論むならば "(?:" が使用可能。
      MIGEMO_OPINDEX_NEST_OUT
        グルーピングの終了を表す閉じ括弧。デフォルトでは ")" 。vimでは "\)" 。
      MIGEMO_OPINDEX_SELECT_IN
        選択の開始を表す開き角括弧。デフォルトでは "[" 。
      MIGEMO_OPINDEX_SELECT_OUT
        選択の終了を表す閉じ角括弧。デフォルトでは "]" 。
      MIGEMO_OPINDEX_NEWLINE
        各文字の間に挿入される「0個以上の空白もしくは改行にマッチする」パター
        ン。デフォルトでは "" であり設定されない。vimでは "\_s*" を指定する。

    デフォルトのメタ文字は特に断りがない限りPerlのそれと同じ意味である。設定に
    成功すると戻り値は0となり、失敗すると0以外になる。

- const unsigned char* migemo_get_operator(migemo* object, int index);
    Migemoオブジェクトが生成する正規表現に使用しているメタ文字(演算子)を取得す
    る。indexについてはmigemo_set_operator()関数を参照。戻り値にはindexの指定
    が正しければメタ文字を格納した文字列へのポインタが、不正であればNULLが返
    る。

- void migemo_setproc_char2int(migemo* object, MIGEMO_PROC_CHAR2INT proc);
    Migemoオブジェクトにコード変換用のプロシージャを設定する。プロシージャにつ
    いての詳細は「型リファレンス」セクションのMIGEMO_PROC_CHAR2INTを参照。

- void migemo_setproc_int2char(migemo* object, MIGEMO_PROC_INT2CHAR proc);
    Migemoオブジェクトにコード変換用のプロシージャを設定する。プロシージャにつ
    いての詳細は「型リファレンス」セクションのMIGEMO_PROC_INT2CHARを参照。

更新箇所
  ● 14-May-2002 (1.1a-beta)
    ドキュメント修正
    cacheを最適化して高速化 (mnode_load())
    wordbuf_add()を最適化して高速化 (wordbuf.c)
    migemo辞書読み込み失敗を検出できなくなっていたバグ修正
    mnodeをまとめて確保することで高速化 (mnode.c他)
    辞書を長さ降順にソート (tools/lensort.pl)
    起動を1割から2割高速化 (mnode.c)
  ● 21-Aug-2001
    main.cのgets()をfgets()に変更

-------------------------------------------------------------------------------
                  生きる事への強い意志が同時に自分と異なる生命をも尊ぶ心となる
                                    MURAOKA Taro/村岡太郎<koron@tka.att.ne.jp>

 vi:set ts=8 sts=2 sw=2 tw=78 et ft=memo:
