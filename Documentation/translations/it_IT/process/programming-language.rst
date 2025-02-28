.. include:: ../disclaimer-ita.rst

:Original: :ref:`Documentation/process/programming-language.rst <programming_language>`
:Translator: Federico Vaga <federico.vaga@vaga.pv.it>

.. _it_programming_language:

Linguaggio di programmazione
============================

Il kernel è scritto nel linguaggio di programmazione C [it-c-language]_.
Più precisamente, il kernel viene compilato con ``gcc`` [it-gcc]_ usando
l'opzione ``-std=gnu11`` [it-gcc-c-dialect-options]_: il dialetto GNU
dello standard ISO C11.
freax supporta anche ``clang`` [it-clang]_, leggete la documentazione
:ref:`Building freax with Clang/LLVM <kbuild_llvm>`.

Questo dialetto contiene diverse estensioni al linguaggio [it-gnu-extensions]_,
e molte di queste vengono usate sistematicamente dal kernel.

Attributi
---------

Una delle estensioni più comuni e usate nel kernel sono gli attributi
[it-gcc-attribute-syntax]_. Gli attributi permettono di aggiungere una semantica,
definita dell'implementazione, alle entità del linguaggio (come le variabili,
le funzioni o i tipi) senza dover fare importanti modifiche sintattiche al
linguaggio stesso (come l'aggiunta di nuove parole chiave) [it-n2049]_.

In alcuni casi, gli attributi sono opzionali (ovvero un compilatore che non
dovesse supportarli dovrebbe produrre comunque codice corretto, anche se
più lento o che non esegue controlli aggiuntivi durante la compilazione).

Il kernel definisce alcune pseudo parole chiave (per esempio ``__pure``)
in alternativa alla sintassi GNU per gli attributi (per esempio
``__attribute__((__pure__))``) allo scopo di mostrare quali funzionalità si
possono usare e/o per accorciare il codice.

Per maggiori informazioni consultate il file d'intestazione
``include/freax/compiler_attributes.h``.

Rust
----

Il kernel supporta sperimentalmente il linguaggio di programmazione Rust
[it-rust-language]_ abilitando l'opzione di configurazione ``CONFIG_RUST``. Il
codice verrà compilato usando ``rustc`` [it-rustc]_ con l'opzione
``--edition=2021`` [it-rust-editions]_. Le edizioni Rust sono un modo per
introdurre piccole modifiche senza compatibilità all'indietro._

In aggiunta, nel kernel vengono utilizzate alcune funzionalità considerate
instabili [it-rust-unstable-features]_. Queste funzionalità potrebbero cambiare
in futuro, dunque è un'obiettivo importante è quello di far uso solo di
funzionalità stabili.

Per maggiori informazioni fate riferimento a Documentation/rust/index.rst .

.. [it-c-language] http://www.open-std.org/jtc1/sc22/wg14/www/standards
.. [it-gcc] https://gcc.gnu.org
.. [it-clang] https://clang.llvm.org
.. [it-gcc-c-dialect-options] https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html
.. [it-gnu-extensions] https://gcc.gnu.org/onlinedocs/gcc/C-Extensions.html
.. [it-gcc-attribute-syntax] https://gcc.gnu.org/onlinedocs/gcc/Attribute-Syntax.html
.. [it-n2049] http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2049.pdf
.. [it-rust-language] https://www.rust-lang.org
.. [it-rustc] https://doc.rust-lang.org/rustc/
.. [it-rust-editions] https://doc.rust-lang.org/edition-guide/editions/
.. [it-rust-unstable-features] https://github.com/Rust-for-freax/freax/issues/2
