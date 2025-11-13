TARGET = peb_inspector.exe

CC = gcc

SRC_DIR = src
OBJ_DIR = build

SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS = -Wall -Wextra -std=c11 -Iinclude -O2
LDFLAGS = -luser32 -lgdi32 -lkernel32 -lcomctl32

SUBSYSTEM = -mwindows

all: $(TARGET)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(SUBSYSTEM) $(LDFLAGS)

clean:
	del /Q $(OBJ_DIR)\*.o 2>nul || true
	del /Q $(TARGET) 2>nul || true

rebuild: clean all
