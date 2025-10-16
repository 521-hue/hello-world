#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NAME_LEN 64
#define PHONE_LEN 32
#define EMAIL_LEN 64

// Data model for a single contact
typedef struct Contact {
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
} Contact;

// Node for singly linked list
typedef struct Node {
    Contact data;
    struct Node *next;
} Node;

// List wrapper to hold head pointer and size
typedef struct ContactList {
    Node *head;
    size_t size;
} ContactList;

static void trim_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n > 0 && s[n-1] == '\n') s[n-1] = '\0';
}

static void read_line(const char *prompt, char *buffer, size_t buffer_len) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buffer, (int)buffer_len, stdin) == NULL) {
        // EOF: treat as empty string
        buffer[0] = '\0';
        return;
    }
    trim_newline(buffer);
}

static void to_lower_str(char *s) {
    for (; s && *s; ++s) *s = (char)tolower((unsigned char)*s);
}

// Initialize the contact list
static void init_list(ContactList *list) {
    list->head = NULL;
    list->size = 0;
}

// Create a new node from a contact
static Node *create_node(const Contact *c) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        perror("malloc");
        return NULL;
    }
    node->data = *c;
    node->next = NULL;
    return node;
}

// Insert contact at tail (append)
static int insert_contact(ContactList *list, const Contact *c) {
    Node *node = create_node(c);
    if (!node) return -1;

    if (!list->head) {
        list->head = node;
    } else {
        Node *cur = list->head;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }
    list->size++;
    return 0;
}

// Delete first contact with matching name (case-insensitive). Returns 1 if deleted, 0 if not found.
static int delete_contact_by_name(ContactList *list, const char *name) {
    if (!list->head) return 0;
    char target[NAME_LEN];
    strncpy(target, name, sizeof(target));
    target[sizeof(target)-1] = '\0';
    to_lower_str(target);

    Node *prev = NULL;
    Node *cur = list->head;
    while (cur) {
        char cur_name[NAME_LEN];
        strncpy(cur_name, cur->data.name, sizeof(cur_name));
        cur_name[sizeof(cur_name)-1] = '\0';
        to_lower_str(cur_name);
        if (strcmp(cur_name, target) == 0) {
            if (prev) prev->next = cur->next; else list->head = cur->next;
            free(cur);
            list->size--;
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

// Find first contact with matching name (case-insensitive). Returns pointer or NULL.
static Contact *find_contact_by_name(ContactList *list, const char *name) {
    char target[NAME_LEN];
    strncpy(target, name, sizeof(target));
    target[sizeof(target)-1] = '\0';
    to_lower_str(target);
    Node *cur = list->head;
    while (cur) {
        char cur_name[NAME_LEN];
        strncpy(cur_name, cur->data.name, sizeof(cur_name));
        cur_name[sizeof(cur_name)-1] = '\0';
        to_lower_str(cur_name);
        if (strcmp(cur_name, target) == 0) {
            return &cur->data;
        }
        cur = cur->next;
    }
    return NULL;
}

// Print a single contact
static void print_contact(const Contact *c) {
    printf("Name : %s\n", c->name);
    printf("Phone: %s\n", c->phone);
    printf("Email: %s\n", c->email);
}

// Print all contacts
static void print_all(const ContactList *list) {
    if (!list->head) {
        printf("[Empty] No contacts.\n");
        return;
    }
    size_t idx = 1;
    const Node *cur = list->head;
    while (cur) {
        printf("--- #%zu ---\n", idx);
        print_contact(&cur->data);
        cur = cur->next;
        idx++;
    }
}

// Free all nodes
static void clear_list(ContactList *list) {
    Node *cur = list->head;
    while (cur) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }
    list->head = NULL;
    list->size = 0;
}

// Gather contact info from user
static Contact input_contact(void) {
    Contact c;
    memset(&c, 0, sizeof(c));
    read_line("Enter name : ", c.name, sizeof(c.name));
    read_line("Enter phone: ", c.phone, sizeof(c.phone));
    read_line("Enter email: ", c.email, sizeof(c.email));
    return c;
}

static void show_menu(void) {
    printf("\n");
    printf("===== Contact Book =====\n");
    printf("1) 录入一个同学信息 (append)\n");
    printf("2) 插入一个同学信息 (append)\n");
    printf("3) 删除指定姓名的同学\n");
    printf("4) 打印所有通讯录\n");
    printf("5) 输出指定姓名的通讯录\n");
    printf("0) 退出\n");
}

int main(void) {
    ContactList list;
    init_list(&list);

    char input[32];
    int running = 1;
    while (running) {
        show_menu();
        read_line("选择功能: ", input, sizeof(input));
        if (strlen(input) == 0) {
            printf("EOF/空输入，退出。\n");
            break;
        }
        switch (input[0]) {
            case '1': {
                // 录入：和插入类似，追加到表尾
                Contact c = input_contact();
                if (insert_contact(&list, &c) == 0) {
                    printf("已添加。当前人数: %zu\n", list.size);
                } else {
                    printf("添加失败。\n");
                }
                break;
            }
            case '2': {
                // 插入（本实现同样采用表尾插入）
                Contact c = input_contact();
                if (insert_contact(&list, &c) == 0) {
                    printf("已插入。当前人数: %zu\n", list.size);
                } else {
                    printf("插入失败。\n");
                }
                break;
            }
            case '3': {
                char name[NAME_LEN];
                read_line("输入要删除的姓名: ", name, sizeof(name));
                int removed = delete_contact_by_name(&list, name);
                if (removed) {
                    printf("已删除。当前人数: %zu\n", list.size);
                } else {
                    printf("未找到姓名为 '%s' 的同学。\n", name);
                }
                break;
            }
            case '4': {
                print_all(&list);
                break;
            }
            case '5': {
                char name[NAME_LEN];
                read_line("输入要查询的姓名: ", name, sizeof(name));
                Contact *found = find_contact_by_name(&list, name);
                if (found) {
                    print_contact(found);
                } else {
                    printf("未找到该姓名。\n");
                }
                break;
            }
            case '0':
                running = 0;
                break;
            default:
                printf("无效选择，请重试。\n");
        }
    }

    clear_list(&list);
    return 0;
}
