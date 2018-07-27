# 本工具清除PSD文件中垃圾数据 by 蘭公子 sRGB.vicp.net

使用方法   src/PSdda.cpp 是源码

双击  PSD大文件修复工具.bat  打开一个命令行窗口

按键盘 ps 再按 TAB箭  psdda 命令会自动补全
再按下空格 ， 从文件管理器 把 psd 文件 拉到窗口里

按回车运行工具，会把修复的文件保存成  Fix_文件名.psd

再用PS 打开 Fix_psd.psd 另存一下，就能缩小文件了

由于PS 本身的BUG，智能对象中垃圾数据即使删除，另存也不能减少大小
可以有两个方法解决
1. 把智能对象合并图层
2. 把智能对象导出，再导入应该就可以

![](https://raw.githubusercontent.com/hongwenjun/psdda/master/psdda.gif)
