# 日历清单软件 - 技术设计方案

## 一、技术选型

- **编程语言**: C++
- **GUI框架**: Qt 5.11.3
- **数据库**: SQLite (本地存储)
- **目标平台**: 统信UOS (ARM架构 Linux)

## 二、核心功能需求

1. **系统托盘**
   - 启动后最小化到托盘
   - 托盘图标右键菜单：显示主窗口、退出
   - 托盘双击显示主窗口

2. **透明日历主窗口**
   - 半透明背景，默认透明度 40%
   - 窗口可调整大小、可拖拽
   - 铺满桌面或可自定义尺寸
   - 无边框窗口设计

3. **日历视图**
   - 显示本月日期网格
   - 月份切换（上/下月箭头）
   - 今日高亮显示
   - 日期单元格显示待办摘要

4. **待办事项管理**
   - 双击日期单元格打开待办编辑对话框
   - 待办内容：标题、描述
   - 勾选完成状态
   - 已完成待办显示删除线效果
   - 数据持久化存储

## 三、技术架构

```
┌─────────────────────────────────────────────────────────┐
│                      main.cpp                           │
│                   (应用程序入口)                         │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│                   MainWindow                            │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐      │
│  │ CalendarView│  │ SystemTray  │  │ SettingsMgr │      │
│  │ (日历视图)  │  │ (系统托盘)  │  │ (设置管理)  │      │
│  └─────────────┘  └─────────────┘  └─────────────┘      │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│                   TodoManager                           │
│                   (待办业务逻辑)                          │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│                   DatabaseManager                      │
│                   (SQLite 数据层)                        │
└─────────────────────────────────────────────────────────┘
```

## 四、类设计

### 4.1 MainWindow
**职责**: 主窗口管理、日历显示、透明度控制

**主要方法**:
- `setupUI()` - 初始化UI布局
- `updateCalendarView()` - 刷新日历显示
- `setOpacity(qreal)` - 设置窗口透明度
- `saveSettings()` / `loadSettings()` - 持久化设置

### 4.2 CalendarView
**职责**: 日历视图容器，管理日期网格

**主要方法**:
- `setCurrentMonth(int year, int month)` - 设置当前月份
- `refreshCells()` - 刷新所有日期单元格
- `loadTodosForMonth(int year, int month)` - 加载当月待办

### 4.3 CalendarCell
**职责**: 单日格子显示待办

**主要方法**:
- `setDate(QDate)` - 设置日期
- `addTodo(const TodoItem&)` - 添加待办显示
- `clearTodos()` - 清空待办

**信号**:
- `doubleClicked(QDate)` - 双击日期信号

### 4.4 TodoDialog
**职责**: 添加/编辑待办对话框

**主要方法**:
- `setDate(QDate)` - 设置关联日期
- `setTodo(const TodoItem&)` - 编辑现有待办
- `getTodo()` - 获取待办数据

### 4.5 TodoItem
**职责**: 待办数据结构

**成员**:
```cpp
struct TodoItem {
    int id;
    QDate date;
    QString title;
    QString description;
    bool completed;
    QDateTime createdAt;
};
```

### 4.6 TodoManager
**职责**: 待办业务逻辑层

**主要方法**:
- `QList<TodoItem> getTodosByDate(QDate)` - 按日期查询
- `QList<TodoItem> getTodosByMonth(int year, int month)` - 按月查询
- `bool addTodo(TodoItem)` - 添加待办
- `bool updateTodo(TodoItem)` - 更新待办
- `bool deleteTodo(int id)` - 删除待办
- `bool toggleCompleted(int id)` - 切换完成状态

### 4.7 DatabaseManager
**职责**: SQLite 数据库操作

**主要方法**:
- `bool initDatabase()` - 初始化数据库
- `bool addTodo(TodoItem)` - 添加待办
- `bool updateTodo(TodoItem)` - 更新待办
- `bool deleteTodo(int id)` - 删除待办
- `TodoItem getTodoById(int id)` - 按ID查询
- `QList<TodoItem> getTodosByDate(QDate)` - 按日期查询
- `QList<TodoItem> getTodosByMonth(int year, int month)` - 按月查询

### 4.8 SystemTrayManager
**职责**: 系统托盘图标和菜单

**主要方法**:
- `setupTrayIcon()` - 初始化托盘
- `createContextMenu()` - 创建右键菜单
- `setWindow(QWidget*)` - 关联主窗口

**信号**:
- `showRequested()` - 显示主窗口
- `quitRequested()` - 退出程序

## 五、数据库设计

**数据库文件**: `~/.config/calendar_todo/data.db`

```sql
-- 待办事项表
CREATE TABLE IF NOT EXISTS todos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    date TEXT NOT NULL,
    title TEXT NOT NULL,
    description TEXT,
    completed INTEGER DEFAULT 0,
    created_at TEXT NOT NULL
);

-- 设置表
CREATE TABLE IF NOT EXISTS settings (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);
```

## 六、UI布局

```
┌────────────────────────────────────────────────────────────┐
│  [◀] [▶]  2026年3月                      [透明度: 40%] [×] │
├────────────────────────────────────────────────────────────┤
│       一       二       三       四       五      六      日  │
├────────────────────────────────────────────────────────────┤
│            │           │           │    │           │         │
│     1      │     2     │     3     │ 4  │     5     │    6    │
│  ┌──────┐  │  ☐ 买菜   │           │    │           │         │
│  │☐ 任务│  │           │           │    │           │         │
│  └──────┘  │           │           │    │           │         │
│            │           │           │    │           │         │
├────────────────────────────────────────────────────────────┤
│     7      │     8      │     9     │  10   │   11  │  12   │
│            │            │           │       │       │       │
├────────────────────────────────────────────────────────────┤
│  ...                                                      │
└────────────────────────────────────────────────────────────┘
```

### 待办显示效果
- 未完成: `☐ 任务标题`
- 已完成: `☑ ~~任务标题~~` (删除线)

## 七、透明度实现

```cpp
// 窗口级别透明度（简单实现）
setWindowOpacity(0.4);  // 40%

// 无边框 + 透明背景（高级实现）
setWindowFlags(Qt::FramelessWindowHint);
setAttribute(Qt::WA_TranslucentBackground);
```

## 八、项目结构

```
/data/home/ZGYD/code/calendar_todo/
├── calendar_todo.pro          # Qt项目文件
├── main.cpp                   # 程序入口
├── mainwindow.h              # 主窗口头文件
├── mainwindow.cpp             # 主窗口实现
├── calendarview.h            # 日历视图头文件
├── calendarview.cpp           # 日历视图实现
├── calendarcell.h            # 日历单元格头文件
├── calendarcell.cpp           # 日历单元格实现
├── tododialog.h              # 待办对话框头文件
├── tododialog.cpp             # 待办对话框实现
├── todoitem.h                # 待办数据结构
├── todomanager.h             # 待办管理器头文件
├── todomanager.cpp            # 待办管理器实现
├── databasemanager.h         # 数据库管理头文件
├── databasemanager.cpp         # 数据库管理实现
├── systemtraymanager.h       # 系统托盘头文件
├── systemtraymanager.cpp      # 系统托盘实现
└── resources/
    └── icons/
        └── app.png           # 应用图标
```

## 九、Qt5 模块依赖

| 模块 | 用途 |
|------|------|
| QtWidgets | 窗口和控件 |
| QtCore | 核心功能 |
| QtGui | 图形效果 |
| QtSql | SQLite 数据库 |

## 十、编译构建

```bash
# 1. 生成Makefile
qmake calendar_todo.pro

# 2. 编译
make -j$(nproc)

# 3. 运行
./calendar_todo
```

## 十一、实现步骤

| 阶段 | 任务 | 说明 |
|------|------|------|
| 1 | 项目搭建 | 创建pro文件，配置Qt模块 |
| 2 | 数据库层 | DatabaseManager实现 |
| 3 | 数据结构 | TodoItem定义 |
| 4 | 待办管理 | TodoManager实现 |
| 5 | 系统托盘 | SystemTrayManager实现 |
| 6 | 主窗口框架 | MainWindow基础框架 |
| 7 | 日历视图 | CalendarView + CalendarCell |
| 8 | 待办对话框 | TodoDialog实现 |
| 9 | 透明度设置 | 集成到主窗口 |
| 10 | 集成测试 | 整体联调 |

## 十二、关键实现细节

### 12.1 双击日期打开待办
```cpp
// CalendarCell 中重写鼠标事件
void CalendarCell::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(m_date);
    }
}
```

### 12.2 完成状态删除线效果
```cpp
// 在显示待办时使用HTML样式
QString displayText = todo.completed 
    ? QString("<s>%1</s>").arg(todo.title)  // 删除线
    : todo.title;
```

### 12.3 统信UOS托盘兼容
```cpp
// 使用QSystemTrayIcon，统信UOS深度兼容
QSystemTrayIcon *tray = new QSystemTrayIcon(this);
tray->setIcon(QIcon(":/icons/app.png"));
tray->setToolTip("日历清单");
```

## 十三、配置存储

**配置文件**: `~/.config/calendar_todo/settings.ini`

```ini
[Window]
opacity=0.4
geometry=@Rect(0, 0, 1920, 1080)

[General]
startMinimized=false
