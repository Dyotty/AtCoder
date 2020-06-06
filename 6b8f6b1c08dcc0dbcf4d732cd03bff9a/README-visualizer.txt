------------------------------------------
6th Asprova Programming Contest Visualizer
------------------------------------------

使い方
・visualizer.htmlをブラウザで開き、input fileに入力データファイルを、output fileに出力データファイルを指定するとガントチャートを表示します。大きいファイルだとやや時間がかかることがあります。Google Chrome, Firefoxなどではドラッグアンドドロップに対応しております。
・各計画の上にマウスを乗せると最早開始時刻、納期などがガントチャート上に表示され、また詳細情報のポップアップがデフォルトでは右下に表示されます。
・表示モードを切り替えると、同一のオーダの分割されたものすべてを表示することや、選択しているオーダが割付け可能なパン焼き機のみを表示することができます。
・何らかを選択した状態で詳細情報のポップアップをドラッグすることで、好きな位置にポップアップを移動させることができます。
・画面右上にあるボタンでチャートを拡大縮小できます。
・拡大時、ドラッグすることで横にスクロールできます。

使用上の注意
・このビジュアライザはすべてのブラウザで作動することを保証していません。推奨ブラウザはGoogle Chrome、動作ブラウザは Firefox、Microsoft Edgeです。
・このビジュアライザで計算された得点が、AtCoder上で計算される得点と一致することは保証されていません。大きな実数を扱った場合、有効桁数の問題で差異が出ることがあります。
・自分のプログラムの出力データの評価はoutput_checker.cppを使用し、ビジュアライザはあくまで出力データの視覚化のための補助的なツールとして使って頂けると幸いです。
・output_checker.cppとビジュアライザの評価結果が異なる場合はoutput_checker.cppの方を信用してください。
・特に、プログラムの出力データが制約を満たしていない場合は利益の横に WA と表示されます。このとき、数値、チャートはあくまでも参考値となります。

ライセンス
・© Asprova Corporation
・MIT License (URL: https://opensource.org/licenses/mit-license.php)
・longcontest visualizer framework を使って作成しています。(URL: https://github.com/kmyk/longcontest-visualizer-framework)
・デザインや初期のコードは rco-contest-2019 を参考にさせて頂きました。(URL: https://github.com/recruit-communications/rco-contest-2019)

------------------------------------------

HOW TO USE
・Open visualizer.html in a browser, and specify both input data file and output data file to display the Gantt chart. Large files may take some time. Google Chrome and Firefox support drag and drop.
・When you hover the mouse over each schedule, information such as earliest start time or delivery time will be displayed on Gantt chart. Details will be displayed on popup as well at the bottom right of the page by default.
・By switching the display mode, you can display all split orders that come from the same order. You can also display all bread machines that can be assigned for the selected order.
・You can move the popup to any position by dragging it while selecting it.
・You can zoom in and out of the chart by clicking buttons at the top right of the page.
・You can scroll the chart by dragging it while zooming in.

USAGE NOTES
・The visualizer is not guaranteed to work on all browsers. The recommended browser is Google Chrome. It also works on Mozilla FireFox and Microsoft Edge.
・The score calculated with this visualizer is not guaranteed to match the score calculated on AtCoder. When dealing with large real numbers, there may be differences due precision limitations.
・Use output_checker.cpp to evaluate the output data of your program. Consider visualizer as an auxiliary tool for visualizing the output data.
・If the evaluation results of output_checker.cpp and the visualizer are different, please trust output_checker.cpp.
・"WA" will be displayed next to the profits when the output data of the program does not meet the constraints. In this case, the numbers and charts are just only values for reference.

License agreement
・© Asprova Corporation
・MIT License (URL: https://opensource.org/licenses/mit-license.php)
・Created using the longcontest visualizer framework. URL: https://github.com/kmyk/longcontest-visualizer-framework)
・Design and initial code were taken from rco-contest-2019.(URL: https://github.com/recruit-communications/rco-contest-2019)
