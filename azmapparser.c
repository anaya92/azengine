#include "azmapparser.h"

#define COLLIDERLIST_ID 2
#define SYMLINK_ID 1
#define DRAWCALL_ID 3

static char *AttributeToString(AXAttribute *attr, int *size)
{
    const char *start = attr->begin;
    const char *limit = attr->limit;
    size_t tsize = limit - start;

    *size = tsize;

    char *buffer = malloc(size);

    memcpy(buffer, start, tsize);
    buffer[tsize] = 0;

    return buffer;
}

static int FaceToInt(char* str)
{
    if (strcmp(str, "up") == 0)
    {
        return 3;
    }
    else if (strcmp(str, "down") == 0)
    {
        return 0;
    }
    else if (strcmp(str, "left") == 0)
    {
        return 1;
    }
    else return 2;
}

static void ResizeStringForAttribute(char *input, AXAttribute *attr)
{
    size_t size = attr->begin - attr->limit;
    input = malloc(size);
}

AzMap AzLoadMap(char *filename)
{
    printf("Loading map %s...\n", filename);

    char* dirpath = GetDirectoryPath(filename);
    printf("%s\n", dirpath);

    printf("Loading XML Schema...\n");
    AXClassContext class_context;
    if (ax_initializeClassParser(&class_context) != RC_OK)
    {
        printf("%s\n", AzXMLGetErrorString(class_context.errorCode));
    }

    char *classText = LoadFileText("resource/schema.xml");
    if (classText == NULL)
    {
        printf("XML Read Error: failed to open schema file!\n");
        exit(-223);
    }
    AXElementClass *schema = ax_classFromString(classText, &class_context);
    free(classText);

    AXParseContext context;
    printf("Initializing XML Parser...\n");
    if (ax_initializeParser(&context, 16 * 1024 * 1024) != RC_OK)
    {
        printf("(%d, %d) %s\n", context.line, context.column, AzXMLGetErrorString(context.errorCode));
        exit(-223);
    }

    char *filecontent = LoadFileText(filename);
    AXElement *root = ax_parse(&context, filecontent, schema, 1);
    if (root == NULL)
    {
        printf("(%d, %d) %s\n", context.line, context.column, AzXMLGetErrorString(context.errorCode));
        exit(-223);
    }
    free(filecontent);

    AzMap map;

    int size;
    char *mapname = AttributeToString(&root->attributes[0], &size);
    char *author = AttributeToString(&root->attributes[1], &size);

    AXElement *opAttr = root->firstChild;

    printf("%i\n", opAttr->id);

    while (opAttr)
    {
        switch (opAttr->id)
        {
            case SYMLINK_ID:
            {
                char* src = AttributeToString(&opAttr->attributes[0], &size);
                char* rel = AttributeToString(&opAttr->attributes[1], &size);

                char srcfinal[128];
                sprintf(srcfinal, "%s/%s", dirpath, src);

                printf("%s\n", srcfinal);

                if (strcmp(rel, "startscript") == 0)
                {
                    strcpy(map.startScript, srcfinal);
                }
                else if (strcmp(rel, "spritesheet") == 0)
                {
                    strcpy(map.pathToSheet, srcfinal);
                }
                else
                {
                    printf("XML Read Error: Unknown link rel.\n");
                    break;
                }

                printf("loaded rel %s, src: %s\n", rel, src);
                break;
            }

            case COLLIDERLIST_ID:
            {
                AXElement* collider = opAttr->firstChild;

                while (collider)
                {
                    char *triggername = AttributeToString(&collider->attributes[0], &size);
                    char *coordString = AttributeToString(&collider->attributes[2], &size);

                    Rectangle newRect;
                    {
                        int x, y, width, height;

                        sscanf(coordString, "%i %i %i %i", &x, &y, &width, &height);
                        newRect.x = x;
                        newRect.y = y;
                        newRect.width = width;
                        newRect.height = height;
                    }

                    if (strcmp(AttributeToString(&collider->attributes[1], &size), "scripted") == 0)
                    {
                        map.eventCollisions.list[map.eventCollisions.count].bounds = newRect;

                        char *script = AttributeToString(&collider->attributes[3], &size);
                        strcpy(map.eventCollisions.list[map.eventCollisions.count].script, script);
                        strcpy(map.eventCollisions.list[map.eventCollisions.count].name, triggername);

                        map.eventCollisions.list[map.eventCollisions.count].hasBeenTriggered = 0;
                        map.eventCollisions.count++;
                    }
                    else if (strcmp(AttributeToString(&collider->attributes[1], &size), "interactable") == 0)
                    {
                        map.interactCollisions.list[map.interactCollisions.count].bounds = newRect;

                        char *script = AttributeToString(&collider->attributes[3], &size);
                        strcpy(map.interactCollisions.list[map.interactCollisions.count].script, script);
                        strcpy(map.interactCollisions.list[map.interactCollisions.count].name, triggername);

                        char* face_req = AttributeToString(&collider->attributes[4], &size);
                        int face_reqInt = FaceToInt(face_req);
                        
                        map.interactCollisions.list[map.interactCollisions.count].req_face = face_reqInt;
                        map.interactCollisions.count++;
                    }
                    else
                    {
                        map.collisions.list[map.collisions.count] = newRect;
                        map.collisions.count++;
                    }

                    collider = collider->nextSibling;
                }
                break;
            }
        }

        opAttr = opAttr->nextSibling;
    }

    strcpy(map.name, mapname);
    strcpy(map.author, author);

    ax_releaseParser(&context);
    ax_releaseClassParser(&class_context);

    printf("Loaded map \'%s\' by %s\n", map.name, map.author);

    return map;
}

void AzUnloadMap(AzMap *map)
{
    free(map);
}

const char *AzXMLGetErrorString(int responseCode)
{
    switch (responseCode)
    {
    case RC_OK:
        return "No error. Why was this called????";
        break;
    case RC_MEMORY:
        return "FATAL: XML Parser ran out of memory!";
        break;
    case RC_EMPTY_NAME:
        return "XML Read Error: name empty or not defined.";
        break;
    case RC_ATTR_DEFINED:
        return "XML Read Error: XML attribute redefinition.";
        break;
    case RC_INVALID_ENTITY_REFERENCE:
        return "XML Read Error: Must be amp, quot, lt, gt, or apos";
        break;
    case RC_UNEXPECTED_END:
        return "XML Read Error: EOF found too early!";
        break;
    case RC_INVALID_CHAR:
        return "XML Read Error: Wrong character.";
        break;
    case RC_OVERFLOW:
        return "XML Read Error: Number to big in char reference";
        break;
    case RC_NO_START_TAG:
        return "XML Read Error: XML does not start with a tag!";
        break;
    case RC_TAG_MISMATCH:
        return "XML Read Error: Invalid close tag.";
        break;
    case RC_INVALID_ATTRIBUTE:
        return "XML Read Error: Attribute not defined in schema";
        break;
    case RC_INVALID_PI:
        return "XML Read Error: Invalid processing instruction (<?xml...)";
        break;
    case RC_INVALID_DOCTYPE:
        return "XML Read Error: Duplicate doctype or doctype after main element";
        break;
    case RC_VERSION_EXPECTED:
        return "XML Read Error: \'version\' is missing in xml declaration.";
        break;
    }
}