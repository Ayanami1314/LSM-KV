use_bf: true, use_cache: true
bf_size: 65536, bf_func_num: 3
value_size: 10, prebuilt data num:1000
Get: 2 us
	throughput: 492975.104757/s
Put: 4 us
	throughput: 238492.725972/s
Del: 0 us
	throughput: 2617801.047120/s
Scan: 26 us
	throughput: 37678.975132/s
value_size: 100, prebuilt data num:1000
Get: 2 us
	throughput: 337256.753566/s
Put: 4 us
	throughput: 210881.484606/s
Del: 0 us
	throughput: 2747252.747253/s
Scan: 28 us
	throughput: 35112.359551/s
value_size: 1000, prebuilt data num:1000
Get: 12 us
	throughput: 79013.282133/s
Put: 10 us
	throughput: 92267.946116/s
Del: 0 us
	throughput: 1592356.687898/s
Scan: 32 us
	throughput: 31172.069825/s
value_size: 10000, prebuilt data num:1000
Get: 76 us
	throughput: 13017.595884/s
Put: 42 us
	throughput: 23264.470501/s
Del: 0 us
	throughput: 2061855.670103/s
Scan: 25 us
	throughput: 38804.811797/s
value_size: 10, prebuilt data num:3000
Get: 2 us
	throughput: 359867.568735/s
Put: 5 us
	throughput: 193873.594416/s
Del: 0 us
	throughput: 3533568.904594/s
Scan: 403 us
	throughput: 2480.158730/s
value_size: 100, prebuilt data num:3000
Get: 3 us
	throughput: 254284.697147/s
Put: 5 us
	throughput: 180082.838106/s
Del: 0 us
	throughput: 3401360.544218/s
Scan: 339 us
	throughput: 2943.254062/s
value_size: 1000, prebuilt data num:3000
Get: 16 us
	throughput: 60943.651500/s
Put: 8 us
	throughput: 111209.964413/s
Del: 0 us
	throughput: 3378378.378378/s
Scan: 1771 us
	throughput: 564.611293/s
value_size: 10000, prebuilt data num:3000
Get: 102 us
	throughput: 9730.125246/s
Put: 46 us
	throughput: 21670.820241/s
Del: 0 us
	throughput: 2227171.492205/s
Scan: 13373 us
	throughput: 74.772952/s
value_size: 10, prebuilt data num:5000
Get: 3 us
	throughput: 313892.899743/s
Put: 6 us
	throughput: 155617.802677/s
Del: 0 us
	throughput: 3436426.116838/s
Scan: 547 us
	throughput: 1826.283878/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 225215.080402/s
Put: 6 us
	throughput: 143122.942608/s
Del: 0 us
	throughput: 3521126.760563/s
Scan: 628 us
	throughput: 1589.850395/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 71927.439599/s
Put: 13 us
	throughput: 72595.281307/s
Del: 0 us
	throughput: 3257328.990228/s
Scan: 1803 us
	throughput: 554.606562/s
value_size: 10000, prebuilt data num:5000
Get: 108 us
	throughput: 9252.636770/s
Put: 46 us
	throughput: 21372.544829/s
Del: 0 us
	throughput: 2257336.343115/s
Scan: 15530 us
	throughput: 64.390256/s


Without BF
use_bf: false, use_cache: true
bf_size: 0, bf_func_num: 0
value_size: 10, prebuilt data num:1000
Get: 2 us
	throughput: 395022.713806/s
Put: 4 us
	throughput: 236854.571293/s
Del: 0 us
	throughput: 3095975.232198/s
Scan: 40 us
	throughput: 24894.199651/s
value_size: 100, prebuilt data num:1000
Get: 2 us
	throughput: 349015.775513/s
Put: 4 us
	throughput: 228310.502283/s
Del: 0 us
	throughput: 3460207.612457/s
Scan: 32 us
	throughput: 30788.177340/s
value_size: 1000, prebuilt data num:1000
Get: 9 us
	throughput: 100838.980316/s
Put: 7 us
	throughput: 129399.585921/s
Del: 0 us
	throughput: 3257328.990228/s
Scan: 30 us
	throughput: 32679.738562/s
value_size: 10000, prebuilt data num:1000
Get: 77 us
	throughput: 12922.167203/s
Put: 39 us
	throughput: 25111.747275/s
Del: 0 us
	throughput: 2557544.757033/s
Scan: 37 us
	throughput: 26838.432636/s
value_size: 10, prebuilt data num:3000
Get: 2 us
	throughput: 337359.152554/s
Put: 5 us
	throughput: 196656.833825/s
Del: 0 us
	throughput: 3533568.904594/s
Scan: 350 us
	throughput: 2850.545880/s
value_size: 100, prebuilt data num:3000
Get: 4 us
	throughput: 242318.503441/s
Put: 5 us
	throughput: 178890.876565/s
Del: 0 us
	throughput: 3424657.534247/s
Scan: 482 us
	throughput: 2071.422653/s
value_size: 1000, prebuilt data num:3000
Get: 13 us
	throughput: 74293.653093/s
Put: 12 us
	throughput: 80625.655083/s
Del: 0 us
	throughput: 1879699.248120/s
Scan: 1507 us
	throughput: 663.464345/s
value_size: 10000, prebuilt data num:3000
Get: 101 us
	throughput: 9827.633142/s
Put: 46 us
	throughput: 21599.205149/s
Del: 0 us
	throughput: 2164502.164502/s
Scan: 13642 us
	throughput: 73.299972/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 340367.597005/s
Put: 6 us
	throughput: 147732.309056/s
Del: 0 us
	throughput: 3412969.283276/s
Scan: 416 us
	throughput: 2401.133335/s
value_size: 100, prebuilt data num:5000
Get: 5 us
	throughput: 187948.727587/s
Put: 7 us
	throughput: 130395.097144/s
Del: 0 us
	throughput: 3484320.557491/s
Scan: 555 us
	throughput: 1800.601401/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 72157.360772/s
Put: 13 us
	throughput: 73003.358154/s
Del: 0 us
	throughput: 3174603.174603/s
Scan: 2114 us
	throughput: 472.976488/s
value_size: 10000, prebuilt data num:5000
Get: 108 us
	throughput: 9253.398773/s
Put: 69 us
	throughput: 14470.523544/s
Del: 0 us
	throughput: 2232142.857143/s
Scan: 17396 us
	throughput: 57.482530/s


Without Cache and BF
use_bf: false, use_cache: false
bf_size: 0, bf_func_num: 0
value_size: 10, prebuilt data num:1000
Get: 2 us
	throughput: 495859.572569/s
Put: 4 us
	throughput: 242424.242424/s
Del: 0 us
	throughput: 3003003.003003/s
Scan: 37 us
	throughput: 26939.655172/s
value_size: 100, prebuilt data num:1000
Get: 3 us
	throughput: 319274.608090/s
Put: 8 us
	throughput: 123411.082315/s
Del: 0 us
	throughput: 1512859.304085/s
Scan: 36 us
	throughput: 27114.967462/s
value_size: 1000, prebuilt data num:1000
Get: 10 us
	throughput: 94447.435280/s
Put: 8 us
	throughput: 121477.162293/s
Del: 0 us
	throughput: 3194888.178914/s
Scan: 47 us
	throughput: 21070.375053/s
value_size: 10000, prebuilt data num:1000
Get: 79 us
	throughput: 12622.915641/s
Put: 49 us
	throughput: 20242.095462/s
Del: 0 us
	throughput: 2364066.193853/s
Scan: 50 us
	throughput: 19980.019980/s
value_size: 10, prebuilt data num:3000
Get: 3 us
	throughput: 322227.234646/s
Put: 6 us
	throughput: 165672.630881/s
Del: 0 us
	throughput: 3174603.174603/s
Scan: 459 us
	throughput: 2174.622159/s
value_size: 100, prebuilt data num:3000
Get: 4 us
	throughput: 244206.207722/s
Put: 5 us
	throughput: 167196.121050/s
Del: 0 us
	throughput: 3389830.508475/s
Scan: 452 us
	throughput: 2207.651721/s
value_size: 1000, prebuilt data num:3000
Get: 13 us
	throughput: 74830.135592/s
Put: 12 us
	throughput: 80096.115338/s
Del: 0 us
	throughput: 2967359.050445/s
Scan: 1803 us
	throughput: 554.480480/s
value_size: 10000, prebuilt data num:3000
Get: 101 us
	throughput: 9850.605714/s
Put: 45 us
	throughput: 21797.414827/s
Del: 0 us
	throughput: 2242152.466368/s
Scan: 13822 us
	throughput: 72.344033/s
value_size: 10, prebuilt data num:5000
Get: 3 us
	throughput: 284705.614395/s
Put: 6 us
	throughput: 143947.027494/s
Del: 0 us
	throughput: 3164556.962025/s
Scan: 583 us
	throughput: 1712.856702/s
value_size: 100, prebuilt data num:5000
Get: 5 us
	throughput: 192038.100359/s
Put: 8 us
	throughput: 111931.945377/s
Del: 0 us
	throughput: 2083333.333333/s
Scan: 611 us
	throughput: 1636.339835/s
value_size: 1000, prebuilt data num:5000
Get: 15 us
	throughput: 65693.094999/s
Put: 18 us
	throughput: 54743.526578/s
Del: 0 us
	throughput: 2352941.176471/s
Scan: 2412 us
	throughput: 414.507772/s
value_size: 10000, prebuilt data num:5000
Get: 107 us
	throughput: 9308.118262/s
Put: 47 us
	throughput: 21249.017233/s
Del: 0 us
	throughput: 2341920.374707/s
Scan: 18329 us
	throughput: 54.555642/s


Test with different BloomFilter size
BloomFilter size: 1024 Bytes
value_size: 10, prebuilt data num:1000
Get: 4 us
	throughput: 220084.952792/s
Put: 9 us
	throughput: 108636.610538/s
Del: 0 us
	throughput: 2202643.171806/s
Scan: 65 us
	throughput: 15190.642564/s
value_size: 100, prebuilt data num:1000
Get: 5 us
	throughput: 177072.635195/s
Put: 10 us
	throughput: 97049.689441/s
Del: 0 us
	throughput: 2398081.534772/s
Scan: 54 us
	throughput: 18518.518519/s
value_size: 1000, prebuilt data num:1000
Get: 14 us
	throughput: 68942.694832/s
Put: 13 us
	throughput: 74727.245554/s
Del: 0 us
	throughput: 2380952.380952/s
Scan: 43 us
	throughput: 22784.233311/s
value_size: 10000, prebuilt data num:1000
Get: 106 us
	throughput: 9420.968438/s
Put: 53 us
	throughput: 18834.874654/s
Del: 0 us
	throughput: 1733102.253033/s
Scan: 55 us
	throughput: 18158.707100/s
value_size: 10, prebuilt data num:3000
Get: 6 us
	throughput: 153404.819979/s
Put: 12 us
	throughput: 82088.327040/s
Del: 0 us
	throughput: 2463054.187192/s
Scan: 714 us
	throughput: 1399.952402/s
value_size: 100, prebuilt data num:3000
Get: 7 us
	throughput: 126992.189980/s
Put: 13 us
	throughput: 72124.053372/s
Del: 0 us
	throughput: 2392344.497608/s
Scan: 1108 us
	throughput: 902.225791/s
value_size: 1000, prebuilt data num:3000
Get: 20 us
	throughput: 49648.981699/s
Put: 15 us
	throughput: 62660.567705/s
Del: 0 us
	throughput: 2087682.672234/s
Scan: 2605 us
	throughput: 383.766670/s
value_size: 10000, prebuilt data num:3000
Get: 143 us
	throughput: 6991.330052/s
Put: 59 us
	throughput: 16702.577208/s
Del: 0 us
	throughput: 1633986.928105/s
Scan: 19937 us
	throughput: 50.156564/s
value_size: 10, prebuilt data num:5000
Get: 3 us
	throughput: 287496.765661/s
Put: 6 us
	throughput: 158679.784195/s
Del: 0 us
	throughput: 3378378.378378/s
Scan: 493 us
	throughput: 2025.644661/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 225687.783520/s
Put: 6 us
	throughput: 146842.878120/s
Del: 0 us
	throughput: 3448275.862069/s
Scan: 437 us
	throughput: 2286.968851/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 71735.496876/s
Put: 10 us
	throughput: 94020.308387/s
Del: 0 us
	throughput: 3278688.524590/s
Scan: 1853 us
	throughput: 539.426697/s
value_size: 10000, prebuilt data num:5000
Get: 107 us
	throughput: 9315.800996/s
Put: 46 us
	throughput: 21303.338233/s
Del: 0 us
	throughput: 2293577.981651/s
Scan: 20000 us
	throughput: 49.998275/s
BloomFilter size: 2048 Bytes
value_size: 10, prebuilt data num:1000
Get: 1 us
	throughput: 512347.576596/s
Put: 3 us
	throughput: 251067.034898/s
Del: 0 us
	throughput: 3115264.797508/s
Scan: 38 us
	throughput: 25859.839669/s
value_size: 100, prebuilt data num:1000
Get: 3 us
	throughput: 321481.386228/s
Put: 4 us
	throughput: 217296.827466/s
Del: 0 us
	throughput: 2881844.380403/s
Scan: 29 us
	throughput: 33568.311514/s
value_size: 1000, prebuilt data num:1000
Get: 10 us
	throughput: 98546.440010/s
Put: 7 us
	throughput: 125944.584383/s
Del: 0 us
	throughput: 3257328.990228/s
Scan: 40 us
	throughput: 24770.869458/s
value_size: 10000, prebuilt data num:1000
Get: 77 us
	throughput: 12973.517159/s
Put: 39 us
	throughput: 25058.260456/s
Del: 0 us
	throughput: 2518891.687657/s
Scan: 31 us
	throughput: 31908.104659/s
value_size: 10, prebuilt data num:3000
Get: 2 us
	throughput: 339639.303060/s
Put: 5 us
	throughput: 189286.390309/s
Del: 0 us
	throughput: 3496503.496503/s
Scan: 333 us
	throughput: 2999.490087/s
value_size: 100, prebuilt data num:3000
Get: 4 us
	throughput: 243831.073832/s
Put: 5 us
	throughput: 176959.830119/s
Del: 0 us
	throughput: 3389830.508475/s
Scan: 443 us
	throughput: 2253.825869/s
value_size: 1000, prebuilt data num:3000
Get: 13 us
	throughput: 75593.789214/s
Put: 10 us
	throughput: 99088.386841/s
Del: 0 us
	throughput: 1941747.572816/s
Scan: 1520 us
	throughput: 657.738957/s
value_size: 10000, prebuilt data num:3000
Get: 101 us
	throughput: 9805.651978/s
Put: 46 us
	throughput: 21485.969662/s
Del: 0 us
	throughput: 2173913.043478/s
Scan: 13290 us
	throughput: 75.243186/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 338615.738860/s
Put: 9 us
	throughput: 101553.772723/s
Del: 0 us
	throughput: 3424657.534247/s
Scan: 564 us
	throughput: 1772.232659/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 223583.597907/s
Put: 7 us
	throughput: 136911.281490/s
Del: 0 us
	throughput: 3496503.496503/s
Scan: 528 us
	throughput: 1891.897005/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 71473.497627/s
Put: 10 us
	throughput: 97914.422794/s
Del: 0 us
	throughput: 3174603.174603/s
Scan: 1867 us
	throughput: 535.523983/s
value_size: 10000, prebuilt data num:5000
Get: 106 us
	throughput: 9360.675242/s
Put: 46 us
	throughput: 21591.277124/s
Del: 0 us
	throughput: 2288329.519451/s
Scan: 25494 us
	throughput: 39.224408/s
BloomFilter size: 3072 Bytes
value_size: 10, prebuilt data num:1000
Get: 4 us
	throughput: 224401.409241/s
Put: 9 us
	throughput: 107735.401853/s
Del: 0 us
	throughput: 2150537.634409/s
Scan: 51 us
	throughput: 19316.206297/s
value_size: 100, prebuilt data num:1000
Get: 5 us
	throughput: 174739.637939/s
Put: 10 us
	throughput: 96861.681519/s
Del: 0 us
	throughput: 2444987.775061/s
Scan: 53 us
	throughput: 18604.651163/s
value_size: 1000, prebuilt data num:1000
Get: 15 us
	throughput: 63857.775961/s
Put: 15 us
	throughput: 63423.606266/s
Del: 0 us
	throughput: 2114164.904863/s
Scan: 52 us
	throughput: 19069.412662/s
value_size: 10000, prebuilt data num:1000
Get: 107 us
	throughput: 9285.154524/s
Put: 53 us
	throughput: 18639.676415/s
Del: 0 us
	throughput: 1574803.149606/s
Scan: 58 us
	throughput: 17179.178835/s
value_size: 10, prebuilt data num:3000
Get: 6 us
	throughput: 153174.542391/s
Put: 11 us
	throughput: 84947.332654/s
Del: 0 us
	throughput: 2427184.466019/s
Scan: 751 us
	throughput: 1329.928716/s
value_size: 100, prebuilt data num:3000
Get: 7 us
	throughput: 125624.984297/s
Put: 12 us
	throughput: 81699.346405/s
Del: 0 us
	throughput: 2352941.176471/s
Scan: 1122 us
	throughput: 890.876533/s
value_size: 1000, prebuilt data num:3000
Get: 20 us
	throughput: 49514.510227/s
Put: 15 us
	throughput: 63443.725416/s
Del: 0 us
	throughput: 2127659.574468/s
Scan: 2404 us
	throughput: 415.829809/s
value_size: 10000, prebuilt data num:3000
Get: 141 us
	throughput: 7079.560812/s
Put: 58 us
	throughput: 17051.170563/s
Del: 0 us
	throughput: 1592356.687898/s
Scan: 11119 us
	throughput: 89.928947/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 346392.323946/s
Put: 6 us
	throughput: 162337.662338/s
Del: 0 us
	throughput: 3533568.904594/s
Scan: 440 us
	throughput: 2269.580808/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 227536.462718/s
Put: 6 us
	throughput: 148522.204070/s
Del: 0 us
	throughput: 3484320.557491/s
Scan: 733 us
	throughput: 1363.661157/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 72581.054893/s
Put: 14 us
	throughput: 67773.636056/s
Del: 0 us
	throughput: 3278688.524590/s
Scan: 1783 us
	throughput: 560.660683/s
value_size: 10000, prebuilt data num:5000
Get: 107 us
	throughput: 9345.759455/s
Put: 46 us
	throughput: 21628.168527/s
Del: 0 us
	throughput: 2369668.246445/s
Scan: 14207 us
	throughput: 70.387292/s
BloomFilter size: 4096 Bytes
value_size: 10, prebuilt data num:1000
Get: 1 us
	throughput: 510099.979596/s
Put: 4 us
	throughput: 243309.002433/s
Del: 0 us
	throughput: 3030303.030303/s
Scan: 46 us
	throughput: 21565.667457/s
value_size: 100, prebuilt data num:1000
Get: 3 us
	throughput: 318268.618714/s
Put: 4 us
	throughput: 227376.080036/s
Del: 0 us
	throughput: 3521126.760563/s
Scan: 35 us
	throughput: 28208.744711/s
value_size: 1000, prebuilt data num:1000
Get: 10 us
	throughput: 94142.456365/s
Put: 7 us
	throughput: 128501.670522/s
Del: 0 us
	throughput: 3205128.205128/s
Scan: 45 us
	throughput: 22079.929344/s
value_size: 10000, prebuilt data num:1000
Get: 77 us
	throughput: 12821.975841/s
Put: 40 us
	throughput: 24936.412149/s
Del: 0 us
	throughput: 2375296.912114/s
Scan: 41 us
	throughput: 24113.817217/s
value_size: 10, prebuilt data num:3000
Get: 2 us
	throughput: 338925.605830/s
Put: 5 us
	throughput: 195045.835771/s
Del: 0 us
	throughput: 3571428.571429/s
Scan: 460 us
	throughput: 2169.668041/s
value_size: 100, prebuilt data num:3000
Get: 3 us
	throughput: 253254.317986/s
Put: 5 us
	throughput: 183486.238532/s
Del: 0 us
	throughput: 3558718.861210/s
Scan: 581 us
	throughput: 1720.548511/s
value_size: 1000, prebuilt data num:3000
Get: 12 us
	throughput: 76988.813525/s
Put: 9 us
	throughput: 106849.022331/s
Del: 0 us
	throughput: 2762430.939227/s
Scan: 1890 us
	throughput: 528.915828/s
value_size: 10000, prebuilt data num:3000
Get: 103 us
	throughput: 9667.150346/s
Put: 49 us
	throughput: 20357.062883/s
Del: 0 us
	throughput: 1344086.021505/s
Scan: 15041 us
	throughput: 66.484101/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 346884.972943/s
Put: 6 us
	throughput: 148964.695367/s
Del: 0 us
	throughput: 3472222.222222/s
Scan: 511 us
	throughput: 1956.870573/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 218007.412252/s
Put: 6 us
	throughput: 148214.021046/s
Del: 0 us
	throughput: 1338688.085676/s
Scan: 644 us
	throughput: 1550.652049/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 72050.262263/s
Put: 13 us
	throughput: 74638.005672/s
Del: 0 us
	throughput: 3300330.033003/s
Scan: 2282 us
	throughput: 438.031661/s
value_size: 10000, prebuilt data num:5000
Get: 106 us
	throughput: 9358.397618/s
Put: 47 us
	throughput: 21165.364997/s
Del: 0 us
	throughput: 2314814.814815/s
Scan: 16346 us
	throughput: 61.174539/s
BloomFilter size: 5120 Bytes
value_size: 10, prebuilt data num:1000
Get: 1 us
	throughput: 507975.210810/s
Put: 7 us
	throughput: 135409.614083/s
Del: 0 us
	throughput: 2433090.024331/s
Scan: 40 us
	throughput: 24503.798089/s
value_size: 100, prebuilt data num:1000
Get: 2 us
	throughput: 353244.551203/s
Put: 5 us
	throughput: 192826.841496/s
Del: 0 us
	throughput: 3355704.697987/s
Scan: 35 us
	throughput: 28019.052956/s
value_size: 1000, prebuilt data num:1000
Get: 9 us
	throughput: 100728.265359/s
Put: 7 us
	throughput: 127975.428718/s
Del: 0 us
	throughput: 3333333.333333/s
Scan: 47 us
	throughput: 20920.502092/s
value_size: 10000, prebuilt data num:1000
Get: 77 us
	throughput: 12951.941820/s
Put: 39 us
	throughput: 25198.437697/s
Del: 0 us
	throughput: 2525252.525253/s
Scan: 39 us
	throughput: 25400.050800/s
value_size: 10, prebuilt data num:3000
Get: 2 us
	throughput: 345101.287228/s
Put: 5 us
	throughput: 191094.974202/s
Del: 0 us
	throughput: 3571428.571429/s
Scan: 415 us
	throughput: 2405.349497/s
value_size: 100, prebuilt data num:3000
Get: 4 us
	throughput: 240743.415668/s
Put: 5 us
	throughput: 177809.388336/s
Del: 0 us
	throughput: 3521126.760563/s
Scan: 465 us
	throughput: 2147.305132/s
value_size: 1000, prebuilt data num:3000
Get: 12 us
	throughput: 76943.200529/s
Put: 9 us
	throughput: 109553.023663/s
Del: 0 us
	throughput: 2923976.608187/s
Scan: 1935 us
	throughput: 516.552939/s
value_size: 10000, prebuilt data num:3000
Get: 103 us
	throughput: 9700.845332/s
Put: 46 us
	throughput: 21651.582731/s
Del: 0 us
	throughput: 2207505.518764/s
Scan: 10518 us
	throughput: 95.068421/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 343029.637761/s
Put: 6 us
	throughput: 160230.732254/s
Del: 0 us
	throughput: 3623188.405797/s
Scan: 473 us
	throughput: 2113.673353/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 222910.768819/s
Put: 6 us
	throughput: 148522.204070/s
Del: 0 us
	throughput: 3484320.557491/s
Scan: 511 us
	throughput: 1953.659204/s
value_size: 1000, prebuilt data num:5000
Get: 14 us
	throughput: 70774.914539/s
Put: 12 us
	throughput: 81406.707913/s
Del: 0 us
	throughput: 3300330.033003/s
Scan: 1903 us
	throughput: 525.339763/s
value_size: 10000, prebuilt data num:5000
Get: 126 us
	throughput: 7889.932289/s
Put: 61 us
	throughput: 16176.518166/s
Del: 0 us
	throughput: 1715265.866209/s
Scan: 18492 us
	throughput: 54.076064/s
BloomFilter size: 6144 Bytes
value_size: 10, prebuilt data num:1000
Get: 4 us
	throughput: 239911.712490/s
Put: 8 us
	throughput: 111894.371713/s
Del: 0 us
	throughput: 1689189.189189/s
Scan: 64 us
	throughput: 15506.280043/s
value_size: 100, prebuilt data num:1000
Get: 5 us
	throughput: 174361.835681/s
Put: 10 us
	throughput: 98173.964265/s
Del: 0 us
	throughput: 2427184.466019/s
Scan: 51 us
	throughput: 19584.802193/s
value_size: 1000, prebuilt data num:1000
Get: 15 us
	throughput: 65452.308176/s
Put: 14 us
	throughput: 69969.213546/s
Del: 0 us
	throughput: 2262443.438914/s
Scan: 51 us
	throughput: 19372.336304/s
value_size: 10000, prebuilt data num:1000
Get: 106 us
	throughput: 9426.243416/s
Put: 54 us
	throughput: 18219.582407/s
Del: 0 us
	throughput: 1661129.568106/s
Scan: 66 us
	throughput: 15149.219815/s
value_size: 10, prebuilt data num:3000
Get: 6 us
	throughput: 153176.888671/s
Put: 12 us
	throughput: 82257.135807/s
Del: 0 us
	throughput: 2336448.598131/s
Scan: 820 us
	throughput: 1218.991894/s
value_size: 100, prebuilt data num:3000
Get: 7 us
	throughput: 125790.910349/s
Put: 11 us
	throughput: 84281.500211/s
Del: 0 us
	throughput: 2380952.380952/s
Scan: 970 us
	throughput: 1029.887330/s
value_size: 1000, prebuilt data num:3000
Get: 20 us
	throughput: 48920.089035/s
Put: 15 us
	throughput: 62762.819306/s
Del: 0 us
	throughput: 2352941.176471/s
Scan: 3126 us
	throughput: 319.885353/s
value_size: 10000, prebuilt data num:3000
Get: 141 us
	throughput: 7084.681784/s
Put: 60 us
	throughput: 16545.607968/s
Del: 0 us
	throughput: 1560062.402496/s
Scan: 15338 us
	throughput: 65.196401/s
value_size: 10, prebuilt data num:5000
Get: 6 us
	throughput: 157225.288115/s
Put: 14 us
	throughput: 71209.855444/s
Del: 0 us
	throughput: 2380952.380952/s
Scan: 1062 us
	throughput: 941.327083/s
value_size: 100, prebuilt data num:5000
Get: 8 us
	throughput: 117814.771616/s
Put: 14 us
	throughput: 69463.739928/s
Del: 0 us
	throughput: 2369668.246445/s
Scan: 845 us
	throughput: 1183.123920/s
value_size: 1000, prebuilt data num:5000
Get: 21 us
	throughput: 46368.209955/s
Put: 18 us
	throughput: 54797.523152/s
Del: 0 us
	throughput: 2272727.272727/s
Scan: 2819 us
	throughput: 354.614959/s
value_size: 10000, prebuilt data num:5000
Get: 154 us
	throughput: 6484.394656/s
Put: 62 us
	throughput: 16040.807815/s
Del: 0 us
	throughput: 1689189.189189/s
Scan: 24298 us
	throughput: 41.154431/s
BloomFilter size: 7168 Bytes
value_size: 10, prebuilt data num:1000
Get: 4 us
	throughput: 218435.998253/s
Put: 9 us
	throughput: 101071.356378/s
Del: 0 us
	throughput: 2123142.250531/s
Scan: 55 us
	throughput: 18076.644975/s
value_size: 100, prebuilt data num:1000
Get: 5 us
	throughput: 176410.401157/s
Put: 10 us
	throughput: 99601.593625/s
Del: 0 us
	throughput: 2358490.566038/s
Scan: 52 us
	throughput: 19204.916459/s
value_size: 1000, prebuilt data num:1000
Get: 15 us
	throughput: 65771.732625/s
Put: 14 us
	throughput: 70372.976777/s
Del: 0 us
	throughput: 2298850.574713/s
Scan: 52 us
	throughput: 18953.752843/s
value_size: 10000, prebuilt data num:1000
Get: 108 us
	throughput: 9247.502943/s
Put: 55 us
	throughput: 17915.046848/s
Del: 0 us
	throughput: 1663893.510815/s
Scan: 44 us
	throughput: 22401.433692/s
value_size: 10, prebuilt data num:3000
Get: 6 us
	throughput: 159103.926685/s
Put: 5 us
	throughput: 189393.939394/s
Del: 0 us
	throughput: 3521126.760563/s
Scan: 386 us
	throughput: 2588.125679/s
value_size: 100, prebuilt data num:3000
Get: 4 us
	throughput: 236529.637164/s
Put: 5 us
	throughput: 180864.532465/s
Del: 0 us
	throughput: 3367003.367003/s
Scan: 513 us
	throughput: 1946.926776/s
value_size: 1000, prebuilt data num:3000
Get: 19 us
	throughput: 51462.566129/s
Put: 16 us
	throughput: 62046.286530/s
Del: 0 us
	throughput: 2159827.213823/s
Scan: 2581 us
	throughput: 387.380687/s
value_size: 10000, prebuilt data num:3000
Get: 141 us
	throughput: 7069.500971/s
Put: 59 us
	throughput: 16758.559434/s
Del: 0 us
	throughput: 1623376.623377/s
Scan: 15121 us
	throughput: 66.131137/s
value_size: 10, prebuilt data num:5000
Get: 6 us
	throughput: 152054.252957/s
Put: 13 us
	throughput: 72040.919242/s
Del: 0 us
	throughput: 2457002.457002/s
Scan: 947 us
	throughput: 1055.420110/s
value_size: 100, prebuilt data num:5000
Get: 8 us
	throughput: 114900.266569/s
Put: 14 us
	throughput: 69993.700567/s
Del: 0 us
	throughput: 2392344.497608/s
Scan: 1355 us
	throughput: 737.969256/s
value_size: 1000, prebuilt data num:5000
Get: 21 us
	throughput: 46120.355680/s
Put: 18 us
	throughput: 55005.500550/s
Del: 0 us
	throughput: 2247191.011236/s
Scan: 3458 us
	throughput: 289.142692/s
value_size: 10000, prebuilt data num:5000
Get: 149 us
	throughput: 6693.319465/s
Put: 62 us
	throughput: 16084.928422/s
Del: 0 us
	throughput: 1589825.119237/s
Scan: 22623 us
	throughput: 44.202216/s
BloomFilter size: 8192 Bytes
value_size: 10, prebuilt data num:1000
Get: 4 us
	throughput: 214965.927900/s
Put: 9 us
	throughput: 106678.045658/s
Del: 0 us
	throughput: 2232142.857143/s
Scan: 44 us
	throughput: 22301.516503/s
value_size: 100, prebuilt data num:1000
Get: 3 us
	throughput: 320163.923929/s
Put: 4 us
	throughput: 226346.763241/s
Del: 0 us
	throughput: 3236245.954693/s
Scan: 46 us
	throughput: 21413.276231/s
value_size: 1000, prebuilt data num:1000
Get: 9 us
	throughput: 100591.477890/s
Put: 7 us
	throughput: 131561.636627/s
Del: 0 us
	throughput: 3194888.178914/s
Scan: 42 us
	throughput: 23331.777881/s
value_size: 10000, prebuilt data num:1000
Get: 77 us
	throughput: 12940.829352/s
Put: 39 us
	throughput: 25194.628505/s
Del: 0 us
	throughput: 2070393.374741/s
Scan: 39 us
	throughput: 25536.261491/s
value_size: 10, prebuilt data num:3000
Get: 2 us
	throughput: 349699.258638/s
Put: 5 us
	throughput: 186462.800671/s
Del: 0 us
	throughput: 3484320.557491/s
Scan: 410 us
	throughput: 2435.519618/s
value_size: 100, prebuilt data num:3000
Get: 4 us
	throughput: 248972.986431/s
Put: 5 us
	throughput: 172801.105927/s
Del: 0 us
	throughput: 3460207.612457/s
Scan: 480 us
	throughput: 2079.650619/s
value_size: 1000, prebuilt data num:3000
Get: 13 us
	throughput: 76657.723266/s
Put: 11 us
	throughput: 90818.272636/s
Del: 0 us
	throughput: 1824817.518248/s
Scan: 1579 us
	throughput: 632.915398/s
value_size: 10000, prebuilt data num:3000
Get: 102 us
	throughput: 9778.344487/s
Put: 45 us
	throughput: 21969.330814/s
Del: 0 us
	throughput: 2227171.492205/s
Scan: 12958 us
	throughput: 77.166567/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 347294.575259/s
Put: 7 us
	throughput: 135980.418820/s
Del: 0 us
	throughput: 3533568.904594/s
Scan: 599 us
	throughput: 1668.195846/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 221483.942414/s
Put: 6 us
	throughput: 146713.615023/s
Del: 0 us
	throughput: 3508771.929825/s
Scan: 670 us
	throughput: 1491.847056/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 72377.230123/s
Put: 14 us
	throughput: 71214.926649/s
Del: 0 us
	throughput: 3378378.378378/s
Scan: 1806 us
	throughput: 553.688395/s
value_size: 10000, prebuilt data num:5000
Get: 107 us
	throughput: 9292.238101/s
Put: 57 us
	throughput: 17363.824209/s
Del: 0 us
	throughput: 2169197.396963/s
Scan: 18509 us
	throughput: 54.025435/s
BloomFilter size: 9216 Bytes
value_size: 10, prebuilt data num:1000
Get: 1 us
	throughput: 507897.810960/s
Put: 4 us
	throughput: 245519.273263/s
Del: 0 us
	throughput: 3125000.000000/s
Scan: 36 us
	throughput: 27464.982148/s
value_size: 100, prebuilt data num:1000
Get: 3 us
	throughput: 320996.372741/s
Put: 4 us
	throughput: 223264.121456/s
Del: 0 us
	throughput: 3267973.856209/s
Scan: 44 us
	throughput: 22356.360385/s
value_size: 1000, prebuilt data num:1000
Get: 12 us
	throughput: 78656.546191/s
Put: 14 us
	throughput: 69328.896284/s
Del: 0 us
	throughput: 2336448.598131/s
Scan: 48 us
	throughput: 20820.320633/s
value_size: 10000, prebuilt data num:1000
Get: 109 us
	throughput: 9170.240004/s
Put: 56 us
	throughput: 17754.105637/s
Del: 0 us
	throughput: 1536098.310292/s
Scan: 62 us
	throughput: 15936.254980/s
value_size: 10, prebuilt data num:3000
Get: 6 us
	throughput: 166223.404255/s
Put: 11 us
	throughput: 89469.446184/s
Del: 0 us
	throughput: 2096436.058700/s
Scan: 814 us
	throughput: 1227.792307/s
value_size: 100, prebuilt data num:3000
Get: 7 us
	throughput: 127718.814259/s
Put: 12 us
	throughput: 80019.204609/s
Del: 0 us
	throughput: 2403846.153846/s
Scan: 654 us
	throughput: 1528.911721/s
value_size: 1000, prebuilt data num:3000
Get: 13 us
	throughput: 75446.644133/s
Put: 9 us
	throughput: 107296.137339/s
Del: 0 us
	throughput: 3215434.083601/s
Scan: 1817 us
	throughput: 550.318359/s
value_size: 10000, prebuilt data num:3000
Get: 102 us
	throughput: 9781.261646/s
Put: 45 us
	throughput: 21754.737094/s
Del: 0 us
	throughput: 2197802.197802/s
Scan: 12579 us
	throughput: 79.495995/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 343524.562006/s
Put: 6 us
	throughput: 157133.878064/s
Del: 0 us
	throughput: 3389830.508475/s
Scan: 440 us
	throughput: 2267.727963/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 230420.055762/s
Put: 7 us
	throughput: 135924.969417/s
Del: 0 us
	throughput: 3436426.116838/s
Scan: 600 us
	throughput: 1666.611113/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 72959.682479/s
Put: 14 us
	throughput: 69584.580057/s
Del: 0 us
	throughput: 3267973.856209/s
Scan: 2036 us
	throughput: 491.122953/s
value_size: 10000, prebuilt data num:5000
Get: 106 us
	throughput: 9353.469482/s
Put: 47 us
	throughput: 21156.856937/s
Del: 0 us
	throughput: 2304147.465438/s
Scan: 21256 us
	throughput: 47.043902/s
BloomFilter size: 10240 Bytes
value_size: 10, prebuilt data num:1000
Get: 2 us
	throughput: 392819.263857/s
Put: 4 us
	throughput: 232504.068821/s
Del: 0 us
	throughput: 3030303.030303/s
Scan: 36 us
	throughput: 27540.622418/s
value_size: 100, prebuilt data num:1000
Get: 2 us
	throughput: 358140.534346/s
Put: 4 us
	throughput: 204206.657137/s
Del: 0 us
	throughput: 3412969.283276/s
Scan: 36 us
	throughput: 27517.886626/s
value_size: 1000, prebuilt data num:1000
Get: 9 us
	throughput: 100756.682687/s
Put: 8 us
	throughput: 124657.192720/s
Del: 0 us
	throughput: 3236245.954693/s
Scan: 33 us
	throughput: 29524.653085/s
value_size: 10000, prebuilt data num:1000
Get: 78 us
	throughput: 12669.004520/s
Put: 38 us
	throughput: 25751.294003/s
Del: 0 us
	throughput: 2518891.687657/s
Scan: 42 us
	throughput: 23512.814484/s
value_size: 10, prebuilt data num:3000
Get: 6 us
	throughput: 156035.451255/s
Put: 11 us
	throughput: 85638.434529/s
Del: 0 us
	throughput: 1210653.753027/s
Scan: 793 us
	throughput: 1260.175921/s
value_size: 100, prebuilt data num:3000
Get: 8 us
	throughput: 124942.214226/s
Put: 12 us
	throughput: 80866.893094/s
Del: 0 us
	throughput: 2444987.775061/s
Scan: 907 us
	throughput: 1101.928375/s
value_size: 1000, prebuilt data num:3000
Get: 20 us
	throughput: 49265.694819/s
Put: 16 us
	throughput: 61865.874783/s
Del: 0 us
	throughput: 2141327.623126/s
Scan: 2925 us
	throughput: 341.772844/s
value_size: 10000, prebuilt data num:3000
Get: 141 us
	throughput: 7065.375210/s
Put: 61 us
	throughput: 16134.497168/s
Del: 0 us
	throughput: 1577287.066246/s
Scan: 18854 us
	throughput: 53.037905/s
value_size: 10, prebuilt data num:5000
Get: 6 us
	throughput: 150690.917858/s
Put: 13 us
	throughput: 72822.604136/s
Del: 0 us
	throughput: 2314814.814815/s
Scan: 1093 us
	throughput: 914.461291/s
value_size: 100, prebuilt data num:5000
Get: 8 us
	throughput: 118120.932210/s
Put: 14 us
	throughput: 69300.069300/s
Del: 0 us
	throughput: 2398081.534772/s
Scan: 1067 us
	throughput: 936.531275/s
value_size: 1000, prebuilt data num:5000
Get: 21 us
	throughput: 46582.476604/s
Put: 18 us
	throughput: 55230.310394/s
Del: 0 us
	throughput: 2347417.840376/s
Scan: 2974 us
	throughput: 336.180785/s
value_size: 10000, prebuilt data num:5000
Get: 150 us
	throughput: 6643.922306/s
Put: 60 us
	throughput: 16440.878601/s
Del: 0 us
	throughput: 1589825.119237/s
Scan: 24910 us
	throughput: 40.144488/s
BloomFilter size: 11264 Bytes
value_size: 10, prebuilt data num:1000
Get: 2 us
	throughput: 482765.279521/s
Put: 4 us
	throughput: 243842.965130/s
Del: 0 us
	throughput: 3134796.238245/s
Scan: 31 us
	throughput: 31426.775613/s
value_size: 100, prebuilt data num:1000
Get: 2 us
	throughput: 359466.551637/s
Put: 4 us
	throughput: 211774.671749/s
Del: 0 us
	throughput: 3521126.760563/s
Scan: 45 us
	throughput: 21982.853374/s
value_size: 1000, prebuilt data num:1000
Get: 10 us
	throughput: 96251.022667/s
Put: 7 us
	throughput: 128982.329421/s
Del: 0 us
	throughput: 3184713.375796/s
Scan: 46 us
	throughput: 21659.085987/s
value_size: 10000, prebuilt data num:1000
Get: 78 us
	throughput: 12811.823776/s
Put: 39 us
	throughput: 25171.163915/s
Del: 0 us
	throughput: 2369668.246445/s
Scan: 35 us
	throughput: 28050.490884/s
value_size: 10, prebuilt data num:3000
Get: 3 us
	throughput: 285836.787195/s
Put: 10 us
	throughput: 92566.879570/s
Del: 0 us
	throughput: 2450980.392157/s
Scan: 681 us
	throughput: 1466.447677/s
value_size: 100, prebuilt data num:3000
Get: 7 us
	throughput: 131941.787283/s
Put: 11 us
	throughput: 86311.065079/s
Del: 0 us
	throughput: 2352941.176471/s
Scan: 754 us
	throughput: 1326.066489/s
value_size: 1000, prebuilt data num:3000
Get: 20 us
	throughput: 49541.248043/s
Put: 15 us
	throughput: 65201.799570/s
Del: 0 us
	throughput: 2074688.796680/s
Scan: 2528 us
	throughput: 395.497655/s
value_size: 10000, prebuilt data num:3000
Get: 143 us
	throughput: 6957.320319/s
Put: 58 us
	throughput: 17059.024224/s
Del: 0 us
	throughput: 1589825.119237/s
Scan: 17543 us
	throughput: 57.002241/s
value_size: 10, prebuilt data num:5000
Get: 6 us
	throughput: 155096.470004/s
Put: 13 us
	throughput: 72124.053372/s
Del: 0 us
	throughput: 2392344.497608/s
Scan: 953 us
	throughput: 1049.130795/s
value_size: 100, prebuilt data num:5000
Get: 8 us
	throughput: 120110.020779/s
Put: 14 us
	throughput: 69720.421111/s
Del: 0 us
	throughput: 2398081.534772/s
Scan: 1093 us
	throughput: 914.645301/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 72497.915685/s
Put: 13 us
	throughput: 72637.466405/s
Del: 0 us
	throughput: 3333333.333333/s
Scan: 1962 us
	throughput: 509.642435/s
value_size: 10000, prebuilt data num:5000
Get: 141 us
	throughput: 7082.408784/s
Put: 61 us
	throughput: 16309.222866/s
Del: 0 us
	throughput: 1703577.512777/s
Scan: 18245 us
	throughput: 54.806623/s
BloomFilter size: 12288 Bytes
value_size: 10, prebuilt data num:1000
Get: 4 us
	throughput: 218192.926185/s
Put: 9 us
	throughput: 106712.197204/s
Del: 0 us
	throughput: 2173913.043478/s
Scan: 57 us
	throughput: 17455.053238/s
value_size: 100, prebuilt data num:1000
Get: 5 us
	throughput: 194291.709573/s
Put: 8 us
	throughput: 113571.834185/s
Del: 0 us
	throughput: 2331002.331002/s
Scan: 56 us
	throughput: 17661.603674/s
value_size: 1000, prebuilt data num:1000
Get: 16 us
	throughput: 62400.938510/s
Put: 14 us
	throughput: 69739.870284/s
Del: 0 us
	throughput: 2331002.331002/s
Scan: 53 us
	throughput: 18744.142455/s
value_size: 10000, prebuilt data num:1000
Get: 109 us
	throughput: 9158.767230/s
Put: 53 us
	throughput: 18687.048007/s
Del: 0 us
	throughput: 1697792.869270/s
Scan: 36 us
	throughput: 27337.342810/s
value_size: 10, prebuilt data num:3000
Get: 6 us
	throughput: 151503.673964/s
Put: 11 us
	throughput: 83375.020844/s
Del: 0 us
	throughput: 2475247.524752/s
Scan: 560 us
	throughput: 1785.586744/s
value_size: 100, prebuilt data num:3000
Get: 7 us
	throughput: 128363.113576/s
Put: 11 us
	throughput: 84260.195484/s
Del: 0 us
	throughput: 2551020.408163/s
Scan: 801 us
	throughput: 1248.018770/s
value_size: 1000, prebuilt data num:3000
Get: 21 us
	throughput: 46796.761664/s
Put: 16 us
	throughput: 62011.658192/s
Del: 0 us
	throughput: 2169197.396963/s
Scan: 2559 us
	throughput: 390.670782/s
value_size: 10000, prebuilt data num:3000
Get: 141 us
	throughput: 7090.242479/s
Put: 59 us
	throughput: 16779.368089/s
Del: 0 us
	throughput: 1522070.015221/s
Scan: 16130 us
	throughput: 61.996165/s
value_size: 10, prebuilt data num:5000
Get: 6 us
	throughput: 156315.945790/s
Put: 13 us
	throughput: 71684.587814/s
Del: 0 us
	throughput: 2352941.176471/s
Scan: 834 us
	throughput: 1198.207482/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 206829.510435/s
Put: 6 us
	throughput: 144780.657304/s
Del: 0 us
	throughput: 3289473.684211/s
Scan: 769 us
	throughput: 1299.714063/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 73107.431370/s
Put: 13 us
	throughput: 72928.821470/s
Del: 0 us
	throughput: 3267973.856209/s
Scan: 2296 us
	throughput: 435.386471/s
value_size: 10000, prebuilt data num:5000
Get: 107 us
	throughput: 9313.415043/s
Put: 46 us
	throughput: 21630.975557/s
Del: 0 us
	throughput: 2247191.011236/s
Scan: 14471 us
	throughput: 69.099809/s
BloomFilter size: 13312 Bytes
value_size: 10, prebuilt data num:1000
Get: 4 us
	throughput: 222687.391440/s
Put: 9 us
	throughput: 104832.791697/s
Del: 0 us
	throughput: 2109704.641350/s
Scan: 55 us
	throughput: 18057.060311/s
value_size: 100, prebuilt data num:1000
Get: 5 us
	throughput: 176214.558847/s
Put: 10 us
	throughput: 97551.458394/s
Del: 0 us
	throughput: 2457002.457002/s
Scan: 57 us
	throughput: 17367.141369/s
value_size: 1000, prebuilt data num:1000
Get: 15 us
	throughput: 64792.437427/s
Put: 14 us
	throughput: 71073.205402/s
Del: 0 us
	throughput: 2320185.614849/s
Scan: 55 us
	throughput: 18008.283811/s
value_size: 10000, prebuilt data num:1000
Get: 108 us
	throughput: 9221.706421/s
Put: 52 us
	throughput: 18870.416848/s
Del: 0 us
	throughput: 1545595.054096/s
Scan: 42 us
	throughput: 23786.869648/s
value_size: 10, prebuilt data num:3000
Get: 6 us
	throughput: 154171.099086/s
Put: 12 us
	throughput: 81579.376734/s
Del: 0 us
	throughput: 2375296.912114/s
Scan: 757 us
	throughput: 1320.132013/s
value_size: 100, prebuilt data num:3000
Get: 4 us
	throughput: 246931.871497/s
Put: 5 us
	throughput: 167448.091092/s
Del: 0 us
	throughput: 3378378.378378/s
Scan: 571 us
	throughput: 1750.087504/s
value_size: 1000, prebuilt data num:3000
Get: 13 us
	throughput: 76687.116564/s
Put: 8 us
	throughput: 111308.993767/s
Del: 0 us
	throughput: 3378378.378378/s
Scan: 1243 us
	throughput: 804.330515/s
value_size: 10000, prebuilt data num:3000
Get: 102 us
	throughput: 9751.825786/s
Put: 45 us
	throughput: 21815.008726/s
Del: 0 us
	throughput: 2242152.466368/s
Scan: 10668 us
	throughput: 93.730112/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 342923.768046/s
Put: 6 us
	throughput: 160076.836882/s
Del: 0 us
	throughput: 3558718.861210/s
Scan: 456 us
	throughput: 2191.252520/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 224683.757611/s
Put: 6 us
	throughput: 148853.825543/s
Del: 0 us
	throughput: 3508771.929825/s
Scan: 591 us
	throughput: 1691.675266/s
value_size: 1000, prebuilt data num:5000
Get: 14 us
	throughput: 70278.513750/s
Put: 13 us
	throughput: 73626.859078/s
Del: 0 us
	throughput: 2118644.067797/s
Scan: 2615 us
	throughput: 382.311224/s
value_size: 10000, prebuilt data num:5000
Get: 107 us
	throughput: 9288.009087/s
Put: 50 us
	throughput: 19917.541379/s
Del: 0 us
	throughput: 2380952.380952/s
Scan: 16871 us
	throughput: 59.271553/s
BloomFilter size: 14336 Bytes
value_size: 10, prebuilt data num:1000
Get: 2 us
	throughput: 391512.019419/s
Put: 4 us
	throughput: 243605.359318/s
Del: 0 us
	throughput: 2958579.881657/s
Scan: 35 us
	throughput: 28417.163967/s
value_size: 100, prebuilt data num:1000
Get: 3 us
	throughput: 319019.970650/s
Put: 4 us
	throughput: 223413.762288/s
Del: 0 us
	throughput: 3448275.862069/s
Scan: 36 us
	throughput: 27770.063871/s
value_size: 1000, prebuilt data num:1000
Get: 10 us
	throughput: 99872.163631/s
Put: 7 us
	throughput: 128849.375081/s
Del: 0 us
	throughput: 3134796.238245/s
Scan: 50 us
	throughput: 19944.156362/s
value_size: 10000, prebuilt data num:1000
Get: 77 us
	throughput: 12930.187333/s
Put: 39 us
	throughput: 25014.383270/s
Del: 0 us
	throughput: 2403846.153846/s
Scan: 34 us
	throughput: 28835.063437/s
value_size: 10, prebuilt data num:3000
Get: 3 us
	throughput: 313558.259125/s
Put: 6 us
	throughput: 158730.158730/s
Del: 0 us
	throughput: 3424657.534247/s
Scan: 465 us
	throughput: 2147.535703/s
value_size: 100, prebuilt data num:3000
Get: 4 us
	throughput: 232514.880952/s
Put: 5 us
	throughput: 168406.871000/s
Del: 0 us
	throughput: 3597122.302158/s
Scan: 419 us
	throughput: 2382.029966/s
value_size: 1000, prebuilt data num:3000
Get: 13 us
	throughput: 74501.769417/s
Put: 9 us
	throughput: 100522.718134/s
Del: 0 us
	throughput: 3389830.508475/s
Scan: 1566 us
	throughput: 638.479907/s
value_size: 10000, prebuilt data num:3000
Get: 101 us
	throughput: 9810.452252/s
Put: 45 us
	throughput: 22056.553002/s
Del: 0 us
	throughput: 2232142.857143/s
Scan: 12637 us
	throughput: 79.127446/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 347971.327163/s
Put: 6 us
	throughput: 159007.791382/s
Del: 0 us
	throughput: 3508771.929825/s
Scan: 467 us
	throughput: 2137.894174/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 223833.825768/s
Put: 6 us
	throughput: 146113.383986/s
Del: 0 us
	throughput: 3448275.862069/s
Scan: 812 us
	throughput: 1231.041954/s
value_size: 1000, prebuilt data num:5000
Get: 20 us
	throughput: 48122.963797/s
Put: 18 us
	throughput: 55285.272004/s
Del: 0 us
	throughput: 2262443.438914/s
Scan: 2522 us
	throughput: 396.358260/s
value_size: 10000, prebuilt data num:5000
Get: 105 us
	throughput: 9481.063472/s
Put: 46 us
	throughput: 21482.738620/s
Del: 0 us
	throughput: 2341920.374707/s
Scan: 19053 us
	throughput: 52.483438/s
BloomFilter size: 15360 Bytes
value_size: 10, prebuilt data num:1000
Get: 4 us
	throughput: 222311.146681/s
Put: 9 us
	throughput: 108049.702863/s
Del: 0 us
	throughput: 2257336.343115/s
Scan: 58 us
	throughput: 17099.863201/s
value_size: 100, prebuilt data num:1000
Get: 5 us
	throughput: 173139.186592/s
Put: 10 us
	throughput: 99127.676447/s
Del: 0 us
	throughput: 3389830.508475/s
Scan: 36 us
	throughput: 27563.395810/s
value_size: 1000, prebuilt data num:1000
Get: 10 us
	throughput: 94384.143464/s
Put: 8 us
	throughput: 123946.455131/s
Del: 0 us
	throughput: 2801120.448179/s
Scan: 35 us
	throughput: 28050.490884/s
value_size: 10000, prebuilt data num:1000
Get: 77 us
	throughput: 12969.125699/s
Put: 39 us
	throughput: 25276.780749/s
Del: 0 us
	throughput: 2457002.457002/s
Scan: 27 us
	throughput: 36737.692873/s
value_size: 10, prebuilt data num:3000
Get: 2 us
	throughput: 345423.143351/s
Put: 5 us
	throughput: 194893.782888/s
Del: 0 us
	throughput: 3597122.302158/s
Scan: 379 us
	throughput: 2636.088045/s
value_size: 100, prebuilt data num:3000
Get: 4 us
	throughput: 242019.409957/s
Put: 5 us
	throughput: 179083.094556/s
Del: 0 us
	throughput: 3484320.557491/s
Scan: 457 us
	throughput: 2185.266930/s
value_size: 1000, prebuilt data num:3000
Get: 13 us
	throughput: 76537.445945/s
Put: 8 us
	throughput: 115593.572997/s
Del: 0 us
	throughput: 3278688.524590/s
Scan: 1911 us
	throughput: 523.286238/s
value_size: 10000, prebuilt data num:3000
Get: 101 us
	throughput: 9849.848903/s
Put: 45 us
	throughput: 21884.232411/s
Del: 0 us
	throughput: 2169197.396963/s
Scan: 13799 us
	throughput: 72.465291/s
value_size: 10, prebuilt data num:5000
Get: 2 us
	throughput: 339939.490771/s
Put: 6 us
	throughput: 147253.718156/s
Del: 0 us
	throughput: 3448275.862069/s
Scan: 461 us
	throughput: 2165.392694/s
value_size: 100, prebuilt data num:5000
Get: 4 us
	throughput: 223164.472216/s
Put: 6 us
	throughput: 146477.222792/s
Del: 0 us
	throughput: 1754385.964912/s
Scan: 476 us
	throughput: 2100.222624/s
value_size: 1000, prebuilt data num:5000
Get: 13 us
	throughput: 72014.979116/s
Put: 10 us
	throughput: 97895.252080/s
Del: 0 us
	throughput: 3267973.856209/s
Scan: 2256 us
	throughput: 443.087612/s
value_size: 10000, prebuilt data num:5000
Get: 140 us
	throughput: 7127.913980/s
Put: 62 us
	throughput: 15998.208201/s
Del: 0 us
	throughput: 1675041.876047/s
Scan: 28011 us
	throughput: 35.699802/s
