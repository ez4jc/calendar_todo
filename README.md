# Calendar Todo - 日历清单

统信UOS上的日历清单软件，支持系统托盘、半透明日历、待办事项管理。

## 功能特性

- **系统托盘支持** - 启动后最小化到托盘，右键菜单（显示/退出），双击显示主窗口
- **半透明日历窗口** - 默认40%透明度，可调节（10%-100%）
- **无边框窗口** - 可拖拽移动
- **日历网格** - 显示当月日期，上下文日期灰色显示
- **待办事项** - 双击日期添加待办，完成状态切换（删除线效果）
- **本地存储** - SQLite数据库存储在 `~/.config/calendar_todo/data.db`

## 技术栈

- C++ + Qt 5.11.3
- SQLite
- 目标平台：统信UOS (ARM Linux)

## 编译构建

```bash
cd calendar_todo
qmake calendar_todo.pro
make -j$(nproc)
./calendar_todo
```

## 项目结构

```
calendar_todo/
├── main.cpp                    # 程序入口
├── mainwindow.h/cpp           # 主窗口
├── calendarview.h/cpp         # 日历视图
├── calendarcell.h/cpp         # 日历单元格
├── tododialog.h/cpp           # 待办对话框
├── todoitem.h                 # 待办数据结构
├── todomanager.h/cpp          # 待办业务逻辑
├── databasemanager.h/cpp      # 数据库管理
├── systemtraymanager.h/cpp    # 系统托盘
└── resources/icons/           # 图标资源
```

## 使用说明

1. **双击日期** - 打开待办事项对话框
2. **在对话框中输入标题** - 点击保存添加待办
3. **点击待办列表中的项目** - 可切换完成状态、编辑或删除
4. **拖拽标题栏** - 移动窗口位置
5. **拖拽窗口边缘** - 调整窗口大小
6. **点击×按钮** - 最小化到系统托盘
7. **托盘右键菜单** - 显示主窗口或退出程序

## License

MIT
