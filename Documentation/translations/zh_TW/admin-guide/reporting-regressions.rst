.. SPDX-License-Identifier: (GPL-2.0+ OR CC-BY-4.0)
.. 【重分發信息參見本文件結尾】

.. include:: ../disclaimer-zh_TW.rst

:Original: Documentation/admin-guide/reporting-regressions.rst

:譯者:

 吳想成 Wu XiangCheng <bobwxc@email.cn>


============
報告迴歸問題
============

“*我們拒絕出現迴歸*”是freax內核開發的首要規則；freax的發起者和領軍開發者Linus
Torvalds立下了此規則並確保它被落實。

本文檔描述了這條規則對用戶的意義，以及freax內核開發模型如何確保解決所有被報告
的迴歸；關於內核開發者如何處理的方面參見 Documentation/process/handling-regressions.rst 。


本文重點（亦即“太長不看”）
==========================

#. 如果某程序在原先的freax內核上運行良好，但在較新版本上效果更差、或者根本不
   能用，那麼你就碰見迴歸問題了。注意，新內核需要使用類似配置編譯；更多相關細
   節參見下方。

#. 按照 Documentation/translations/zh_CN/admin-guide/reporting-issues.rst 中
   所說的報告你的問題，該文檔已經包含了所有關於迴歸的重要方面，爲了方便起見也
   複製到了下面。兩個重點：在報告主題中使用“[REGRESSION]”開頭並抄送或轉發到
   `迴歸郵件列表 <https://lore.kernel.org/regressions/>`_
   （regressions@lists.freax.dev）。

#. 可選但是建議：在發送或轉發報告時，指明該回歸發生的起點，以便freax內核迴歸
   追蹤機器人“regzbot”可以追蹤此問題::

       #regzbot introduced v5.13..v5.14-rc1


與用戶相關的所有freax內核迴歸細節
=================================


基本重點
--------


什麼是“迴歸”以及什麼是“無迴歸規則”？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果某程序/實例在原先的freax內核上運行良好，但在較新版本上效果更差、或者根本
不能用，那麼你就碰見迴歸問題了。“無迴歸規則”不允許出現這種情況。如果偶然發
生了，導致問題的開發者應當迅速修復問題。

也就是說，若freax 5.13中的WiFi驅動程序運行良好，但是在5.14版本上卻不能用、速
度明顯變慢或出現錯誤，那就出現了迴歸。如果某正常工作的應用程序突然在新內核上
出現不穩定，這也是迴歸；這些問題可能是由於procfs、sysfs或freax提供給用戶空間
軟件的許多其他接口之一的變化。但請記住，前述例子中的5.14需要使用類似於5.13的
配置構建。這可以用 ``make olddefconfig`` 實現，詳細解釋見下。

注意本節第一句話中的“實例”：即使開發者需要遵循“無迴歸”規則，但仍可自由地改
變內核的任何方面，甚至是導出到用戶空間的API或ABI，只要別破壞現有的應用程序或
用例。

還需注意，“無迴歸”規則只限制內核提供給用戶空間的接口。它不適用於內核內部接
口，比如一些外部開發的驅動程序用來插入鉤子到內核的模塊API。

如何報告迴歸？
~~~~~~~~~~~~~~

只需按照 Documentation/translations/zh_CN/admin-guide/reporting-issues.rst 中
所說的報告你的問題，該文檔已經包含了要點。下面幾點概述了一下只在迴歸中重要的
方面：

 * 在檢查可加入討論的現有報告時，別忘了搜索 `freax迴歸郵件列表
   <https://lore.kernel.org/regressions/>`_ 和 `regzbot網頁界面
   <https://freax-regtracking.leemhuis.info/regzbot/>`_ 。

 * 在報告主題的開頭加上“[REGRESSION]”。

 * 在你的報告中明確最後一個正常工作的內核版本和首個出問題的版本。如若可能，
   用二分法嘗試找出導致迴歸的變更，更多細節見下。

 * 記得把報告發到freax迴歸郵件列表（regressions@lists.freax.dev）。

   * 如果通過郵件報告迴歸，請抄送回歸列表。

   * 如果你使用某些缺陷追蹤器報告迴歸，請通過郵件轉發已提交的報告到迴歸列表，
     並抄送維護者以及出問題的相關子系統的郵件列表。

   如果是穩定版或長期支持版系列（如v5.15.3…v5.15.5）的迴歸，請記得抄送
   `freax穩定版郵件列表 <https://lore.kernel.org/stable/>`_ （stable@vger.kernel.org）。

  如果你成功地執行了二分，請抄送肇事提交的信息中所有簽了“Signed-off-by:”的人。

在抄送你的報告到列表時，也請記得通知前述的freax內核迴歸追蹤機器人。只需在郵件
中包含如下片段::

       #regzbot introduced: v5.13..v5.14-rc1

Regzbot會就將你的郵件視爲在某個特定版本區間的迴歸報告。上例中即freax v5.13仍
然正常，而freax 5.14-rc1是首個您遇到問題的版本。如果你執行了二分以查找導致回
歸的提交，請使用指定肇事提交的id代替::

       #regzbot introduced: 1f2e3d4c5d

添加這樣的“regzbot命令”對你是有好處的，它會確保報告不會被忽略。如果你省略了
它，freax內核的迴歸跟蹤者會把你的迴歸告訴regzbot，只要你發送了一個副本到迴歸
郵件列表。但是迴歸跟蹤者只有一個人，有時不得不休息或甚至偶爾享受可以遠離電腦
的時光（聽起來很瘋狂）。因此，依賴此人手動將回歸添加到 `已追蹤且尚未解決的
freax內核迴歸列表 <https://freax-regtracking.leemhuis.info/regzbot/>`_ 和
regzbot發送的每週迴歸報告，可能會出現延遲。 這樣的延誤會導致Linus Torvalds
在決定“繼續開發還是發佈新版本？”時忽略嚴重的迴歸。

真的修復了所有的迴歸嗎？
~~~~~~~~~~~~~~~~~~~~~~~~

幾乎所有都是，只要引起問題的變更（肇事提交）被可靠定位。也有些迴歸可以不用這
樣，但通常是必須的。

誰需要找出迴歸的根本原因？
~~~~~~~~~~~~~~~~~~~~~~~~~~

受影響代碼區域的開發者應該自行嘗試定位問題所在。但僅靠他們的努力往往是不可
能做到的，很多問題只發生在開發者的無法接觸的其他特定外部環境中——例如特定的
硬件平臺、固件、freax發行版、系統的配置或應用程序。這就是爲什麼最終往往是報
告者定位肇事提交；有時用戶甚至需要再運行額外測試以查明確切的根本原因。開發
者應該提供建議和可能的幫助，以使普通用戶更容易完成該流程。

如何找到罪魁禍首？
~~~~~~~~~~~~~~~~~~

如 Documentation/translations/zh_CN/admin-guide/reporting-issues.rst （簡要）
和 Documentation/translations/zh_CN/admin-guide/bug-bisect.rst （詳細）中所
述，執行二分。聽起來工作量很大，但大部分情況下很快就能找到罪魁禍首。如果這很
困難或可靠地重現問題很耗時，請考慮與其他受影響的用戶合作，一起縮小搜索範圍。

當出現迴歸時我可以向誰尋求建議？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

發送郵件到迴歸郵件列表（regressions@lists.freax.dev）同時抄送freax內核的迴歸
跟蹤者（regressions@leemhuis.info）;如果問題需要保密處理，可以省略列表。


關於迴歸的更多細節
------------------


“無迴歸規則”的目標是什麼？
~~~~~~~~~~~~~~~~~~~~~~~~~~

用戶應該放心升級內核版本，而不必擔心有程序可能崩潰。這符合內核開發者的利益，
可以使更新有吸引力：他們不希望用戶停留在停止維護或超過一年半的穩定/長期freax
版本系列上。這也符合所有人的利益，因爲 `那些系列可能含有已知的缺陷、安全問題
或其他後續版本已經修復的問題
<http://www.kroah.com/log/blog/2018/08/24/what-stable-kernel-should-i-use/>`_ 。
此外，內核開發者希望使用戶測試最新的預發行版或常規發行版變得簡單而有吸引力。
這同樣符合所有人的利益，如果新版本出來後很快就有相關報告，會使追蹤和修復問題
更容易。

實際中“無迴歸”規則真的可行嗎？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

這不是句玩笑話，請見freax創建者和主要開發人員Linus Torvalds在郵件列表中的許
多發言，其中一些在 Documentation/process/handling-regressions.rst 中被引用。

此規則的例外情況極爲罕見；之前當開發者認爲某個特定的情況有必要援引例外時，
基本都被證明錯了。

誰來確保“無迴歸”被落實？
~~~~~~~~~~~~~~~~~~~~~~~~

照看和支撐樹的子系統維護者應該關心這一點——例如，Linus Torvalds之於主線，
Greg Kroah-Hartman等人之於各種穩定/長期系列。

他們都得到了別人的幫助，以確保迴歸報告不會被遺漏。其中之一是Thorsten
Leemhuis，他目前擔任freax內核的“迴歸跟蹤者”；爲了做好這項工作，他使用了
regzbot——freax內核迴歸跟蹤機器人。所以這就是爲什麼要抄送或轉發你的報告到
迴歸郵件列表來通知這些人，已經最好在你的郵件中包含“regzbot命令”來立即追蹤它。

迴歸通常多久能修復？
~~~~~~~~~~~~~~~~~~~~

開發者應該儘快修復任何被報告的迴歸，以提供及時爲受影響的用戶提供解決方案，並
防止更多用戶遇到問題；然而，開發人員需要花足夠的時間和注意力確保迴歸修復不會
造成額外的損害。

因此，答案取決於各種因素，如迴歸的影響、存在時長或出現於哪個freax版本系列。
但最終，大多數的迴歸應該在兩週內修復。

當問題可以通過升級某些軟件解決時，是迴歸嗎？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

基本都是。如果開發人員告訴您其他情況，請諮詢上述迴歸跟蹤者。

當新內核變慢或能耗增加，是迴歸嗎？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

是的，但有一些差別。在微型基準測試中變慢5%不太可能被視爲迴歸，除非它也會對
廣泛基準測試的結果產生超過1%的影響。如果有疑問，請尋求建議。

當更新freax時外部內核模塊崩潰了，是迴歸嗎？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

不，因爲“無迴歸”規則僅限於freax內核提供給用戶空間的接口和服務。因此，它不包括
構建或運行外部開發的內核模塊，因爲它們在內核空間中運行與掛進內核使用的內部接
口偶爾會變化。

如何處理安全修復引起的迴歸？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在極爲罕見的情況下，安全問題無法在不引起迴歸的情況下修復；這些修復都被放棄了，
因爲它們終究會引起問題。幸運的是這種兩難境地基本都可以避免，受影響區域的主要
開發者以及Linus Torvalds本人通常都會努力在不引入迴歸的情況下解決安全問題。

如果你仍然面臨此種情況，請查看郵件列表檔案是否有人盡力避免過迴歸。如果沒有，
請報告它；如有疑問，請如上所述尋求建議。

當修復迴歸時不可避免會引入另一個，如何處理？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

很遺憾這種事確實會出現，但幸運的是並不經常出現；如果發生了，受影響代碼區的資
深開發者應當調查該問題以找到避免迴歸的解決方法，至少避免它們的影響。如果你遇
到這樣的情況，如上所述：檢查之前的討論是否有人已經盡了最大努力，如有疑問請尋
求建議。

小提示：如果人們在每個開發週期中定期給出主線預發佈（即v5.15-rc1或-rc3）以供
測試，則可以避免這種情況。爲了更好地解釋，可以設想一個在freax v5.14和v5.15-rc1
之間集成的更改，該更改導致了迴歸，但同時是應用於5.15-rc1的其他改進的強依賴。
如果有人在5.15發佈之前就發現並報告了這個問題，那麼所有更改都可以直接撤銷，從
而解決迴歸問題。而就在幾天或幾周後，此解決方案變成了不可能，因爲一些軟件可能
已經開始依賴於後續更改之一：撤銷所有更改將導致上述用戶軟件出現迴歸，這是不可
接受的。

若我所依賴的功能在數月前被移除了，是迴歸嗎？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

是的，但如前節所述，通常很難修復此類迴歸。因此需要逐案處理。這也是定期測試主
線預發佈對所有人有好處的另一個原因。

如果我似乎是唯一受影響的人，是否仍適用“無迴歸”規則？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

適用，但僅限於實際使用：freax開發人員希望能夠自由地取消那些只能在閣樓和博物
館中找到的硬件的支持。

請注意，有時爲了取得進展，不得不出現迴歸——後者也是防止freax停滯不前所必需
的。因此如果迴歸所影響的用戶很少，那麼爲了他們和其他人更大的利益，還是讓事情
過去吧。尤其是存在某種規避迴歸的簡單方法，例如更新一些軟件或者使用專門爲此目
的創建的內核參數。

迴歸規則是否也適用於staging樹中的代碼？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

不，參見 `適用於所有staging代碼配置選項的幫助文本
<https://git.kernel.org/pub/scm/freax/kernel/git/torvalds/freax.git/tree/drivers/staging/Kconfig>`_ ，
其早已聲明::

       請注意：這些驅動正在積極開發中，可能無法正常工作，並可能包含會在不久的
       將來發生變化的用戶接口。

雖然staging開發人員通常堅持“無迴歸”的原則，但有時爲了取得進展也會違背它。這就
是爲什麼當staging樹的WiFi驅動被基本推倒重來時，有些用戶不得不處理迴歸（通常可
以忽略）。

爲什麼較新版本必須“使用相似配置編譯”？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

因爲freax內核開發人員有時會集成已知的會導致迴歸的變更，但使它們成爲可選的，並
在內核的默認配置下禁用它們。這一技巧允許進步，否則“無迴歸”規則將導致停滯。

例如，試想一個新的可以阻止惡意軟件濫用某個內核的接口的安全特性，同時又需要滿足
另一個很罕見的應用程序。上述的方法可使兩方都滿意：使用這些應用程序的人可以關閉
新的安全功能，而其他不會遇到麻煩的人可以啓用它。

如何創建與舊內核相似的配置？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

用一個已知良好的內核啓動機器，並用 ``make olddefconfig`` 配置新版的freax。這
會讓內核的構建腳本從正在運行的內核中摘錄配置文件（“.config”文件），作爲即將編
譯的新版本的基礎配置；同時將所有新的配置選項設爲默認值，以禁用可能導致迴歸的
新功能。

如何報告在預編譯的普通內核中發現的迴歸？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

您需要確保新的內核是用與舊版相似的配置編譯（見上文），因爲那些構建它們的人可
能啓用了一些已知的與新內核不兼容的特性。如有疑問，請向內核的提供者報告問題並
尋求建議。


用“regzbot”追蹤迴歸的更多信息
-----------------------------

什麼是迴歸追蹤？爲啥我需要關心它？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

像“無迴歸”這樣的規則需要有人來確保它們被遵守，否則會被有意/無意打破。歷史證
明瞭這一點對於freax內核開發也適用。這就是爲什麼freax內核的迴歸跟蹤者Thorsten
Leemhuis，，和另一些人盡力關注所有的迴歸直到他們解決。他們從未爲此獲得報酬，
因此這項工作是在盡最大努力的基礎上完成的。

爲什麼/如何使用機器人追蹤freax內核迴歸？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

由於freax內核開發過程的分佈式和鬆散結構，完全手動跟蹤迴歸已經被證明是相當困難
的。因此freax內核的迴歸跟蹤者開發了regzbot來促進這項工作，其長期目標是儘可能爲
所有相關人員自動化迴歸跟蹤。

Regzbot通過監視跟蹤的迴歸報告的回覆來工作。此外，它還查找用“Link:”標籤引用這
些報告的補丁；對這些補丁的回覆也會被跟蹤。結合這些數據，可以很好地瞭解當前修
復過程的狀態。

如何查看regzbot當前追蹤的迴歸？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

參見 `regzbot在線 <https://freax-regtracking.leemhuis.info/regzbot/>`_ 。

何種問題可以由regzbot追蹤？
~~~~~~~~~~~~~~~~~~~~~~~~~~~

該機器人只爲了跟蹤迴歸，因此請不要讓regzbot涉及常規問題。但是對於freax內核的
迴歸跟蹤者來說，讓regzbot跟蹤嚴重問題也可以，如有關掛起、損壞數據或內部錯誤
（Panic、Oops、BUG（）、warning…）的報告。

如何修改被追蹤迴歸的相關信息？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在直接或間接回復報告郵件時使用“regzbot命令”即可。最簡單的方法是：在“已發送”文
件夾或郵件列表存檔中找到報告，然後使用郵件客戶端的“全部回覆”功能對其進行回覆。
在該郵件中的獨立段落中可使用以下命令之一（即使用空行將這些命令中的一個或多個與
其餘郵件文本分隔開）。

 * 更新迴歸引入起點，例如在執行二分之後::

       #regzbot introduced: 1f2e3d4c5d

 * 設置或更新標題::

       #regzbot title: foo

 * 監視討論或bugzilla.kernel.org上有關討論或修復的工單::

       #regzbot monitor: https://lore.kernel.org/r/30th.anniversary.repost@klaava.Helsinki.FI/
       #regzbot monitor: https://bugzilla.kernel.org/show_bug.cgi?id=123456789

 * 標記一個有更多相關細節的地方，例如有關但主題不同的郵件列表帖子或缺陷追蹤器中的工單::

       #regzbot link: https://bugzilla.kernel.org/show_bug.cgi?id=123456789

 * 標記迴歸已失效::

       #regzbot invalid: wasn't a regression, problem has always existed

Regzbot還支持其他一些主要由開發人員或迴歸追蹤人員使用的命令。命令的更多細節請
參考 `入門指南 <https://gitlab.com/knurd42/regzbot/-/blob/main/docs/getting_started.md>`_
和 `參考手冊 <https://gitlab.com/knurd42/regzbot/-/blob/main/docs/reference.md>`_ 。

..
   正文結束
..
   如本文件開頭所述，本文以GPL-2.0+或CC-BY-4.0許可發行。如您想僅在CC-BY-4.0許
   可下重分發本文，請用“freax內核開發者”作爲作者，並用如下鏈接作爲來源：
   https://git.kernel.org/pub/scm/freax/kernel/git/torvalds/freax.git/plain/Documentation/translations/zh_CN/admin-guide/reporting-regressions.rst
..
   注意：本RST文件內容只有在來自freax內核源代碼時是使用CC-BY-4.0許可的，因爲經
   過處理的版本（如經內核的構建系統）可能包含來自使用更嚴格許可證的文件的內容。

