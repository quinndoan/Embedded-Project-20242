# DFD Level 2 - File Operations Detail

## Detailed File Management Processes

```mermaid
graph TB
    %% External Input
    User_Input[User Commands]

    %% File Operation Sub-Processes
    List_Files[3.1<br/>List Files<br/>Process]
    Create_Dir[3.2<br/>Create Directory<br/>Process]
    Delete_Dir[3.3<br/>Delete Directory<br/>Process]
    Create_File[3.4<br/>Create File<br/>Process]
    Delete_File[3.5<br/>Delete File<br/>Process]
    Update_File[3.6<br/>Update File<br/>Process]

    %% FATFS Operations
    FATFS_Mount[5.1<br/>Mount File System<br/>Process]
    FATFS_Open[5.2<br/>Open File<br/>Process]
    FATFS_Read[5.3<br/>Read File<br/>Process]
    FATFS_Write[5.4<br/>Write File<br/>Process]
    FATFS_Close[5.5<br/>Close File<br/>Process]
    FATFS_Mkdir[5.6<br/>Make Directory<br/>Process]
    FATFS_Unlink[5.7<br/>Delete File/Dir<br/>Process]

    %% SD Card Operations
    SD_Init[4.1<br/>SD Initialize<br/>Process]
    SD_Read_Sector[4.2<br/>Read Sector<br/>Process]
    SD_Write_Sector[4.3<br/>Write Sector<br/>Process]
    SD_Status[4.4<br/>SD Status<br/>Process]

    %% Data Stores
    File_System[FATFS File System<br/>Data Store]
    File_Content[File Content<br/>Buffer]
    Directory_Info[Directory Information<br/>Buffer]

    %% Data Flows
    User_Input -->|Option 1| List_Files
    User_Input -->|Option 2| Create_Dir
    User_Input -->|Option 3| Delete_Dir
    User_Input -->|Option 4| Create_File
    User_Input -->|Option 5| Delete_File
    User_Input -->|Option 6| Update_File

    %% List Files Flow
    List_Files -->|Mount Request| FATFS_Mount
    FATFS_Mount -->|Mount Status| File_System
    List_Files -->|Read Directory| FATFS_Read
    FATFS_Read -->|Directory Data| Directory_Info
    Directory_Info -->|File List| List_Files

    %% Create Directory Flow
    Create_Dir -->|Create Request| FATFS_Mkdir
    FATFS_Mkdir -->|Directory Created| File_System

    %% Delete Directory Flow
    Delete_Dir -->|Delete Request| FATFS_Unlink
    FATFS_Unlink -->|Directory Deleted| File_System

    %% Create File Flow
    Create_File -->|Create Request| FATFS_Open
    FATFS_Open -->|File Handle| File_Content
    Create_File -->|Write Content| FATFS_Write
    FATFS_Write -->|Content Written| File_System
    Create_File -->|Close Request| FATFS_Close

    %% Delete File Flow
    Delete_File -->|Delete Request| FATFS_Unlink
    FATFS_Unlink -->|File Deleted| File_System

    %% Update File Flow
    Update_File -->|Open Request| FATFS_Open
    FATFS_Open -->|File Handle| File_Content
    Update_File -->|Append Content| FATFS_Write
    FATFS_Write -->|Content Updated| File_System
    Update_File -->|Close Request| FATFS_Close

    %% FATFS to SD Interface
    FATFS_Mount -->|Init Request| SD_Init
    FATFS_Read -->|Read Request| SD_Read_Sector
    FATFS_Write -->|Write Request| SD_Write_Sector
    FATFS_Mkdir -->|Write Request| SD_Write_Sector
    FATFS_Unlink -->|Write Request| SD_Write_Sector

    %% SD Card Interface
    SD_Init -->|SPI Commands| SDCard[SD Memory Card]
    SD_Read_Sector -->|SPI Read| SDCard
    SD_Write_Sector -->|SPI Write| SDCard
    SDCard -->|Sector Data| SD_Read_Sector
    SDCard -->|Write Status| SD_Write_Sector

    %% Styling
    classDef process fill:#f3e5f5,stroke:#4a148c,stroke-width:2px
    classDef datastore fill:#fff3e0,stroke:#e65100,stroke-width:2px
    classDef external fill:#e1f5fe,stroke:#01579b,stroke-width:2px

    class List_Files,Create_Dir,Delete_Dir,Create_File,Delete_File,Update_File,FATFS_Mount,FATFS_Open,FATFS_Read,FATFS_Write,FATFS_Close,FATFS_Mkdir,FATFS_Unlink,SD_Init,SD_Read_Sector,SD_Write_Sector,SD_Status process
    class File_System,File_Content,Directory_Info datastore
    class User_Input,SDCard external
```

## Detailed Process Descriptions:

### 3.1 List Files Process

- **Input**: User selection for file listing
- **Output**: File and directory list
- **Functions**: `SD_List_File()`
- **Data Flow**: Mount → Read Directory → Display Files

### 3.2 Create Directory Process

- **Input**: Directory name from user
- **Output**: New directory created
- **Functions**: `SD_creatSubDir()`
- **Data Flow**: Validate → Create → Mount → Verify

### 3.3 Delete Directory Process

- **Input**: Directory name to delete
- **Output**: Directory and contents deleted
- **Functions**: `SD_deleteFolder()`, `delete_node()`
- **Data Flow**: Open Directory → Delete Contents → Delete Directory

### 3.4 Create File Process

- **Input**: File name and content
- **Output**: New file created with content
- **Functions**: `f_open()`, `f_write()`, `f_close()`
- **Data Flow**: Create File → Write Content → Close File

### 3.5 Delete File Process

- **Input**: File name to delete
- **Output**: File deleted
- **Functions**: `f_unlink()`
- **Data Flow**: Validate → Delete → Update File System

### 3.6 Update File Process

- **Input**: File name and new content
- **Output**: File updated with appended content
- **Functions**: `f_open()`, `f_lseek()`, `f_puts()`, `f_close()`
- **Data Flow**: Open File → Seek to End → Append → Close

## FATFS Sub-Processes:

### 5.1 Mount File System

- **Function**: `f_mount(&fs, "/", 1)`
- **Purpose**: Initialize and mount FAT file system

### 5.2-5.5 File I/O Operations

- **Open**: `f_open()` - Open file for read/write
- **Read**: `f_read()` - Read file content
- **Write**: `f_write()` - Write data to file
- **Close**: `f_close()` - Close file handle

### 5.6-5.7 Directory Operations

- **Make Directory**: `f_mkdir()` - Create new directory
- **Delete**: `f_unlink()` - Delete file or directory

## SD Card Sub-Processes:

### 4.1 SD Initialize

- **Function**: `SD_disk_initialize()`
- **Purpose**: Initialize SD card and SPI interface

### 4.2-4.3 Sector Operations

- **Read Sector**: `SD_disk_read()` - Read 512-byte sectors
- **Write Sector**: `SD_disk_write()` - Write 512-byte sectors

### 4.4 SD Status

- **Function**: `SD_disk_status()`
- **Purpose**: Check SD card status and errors
