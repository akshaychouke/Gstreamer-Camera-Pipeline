#include <gst/gst.h>
#include <stdio.h>

typedef struct _CustomData
{
    GstElement *pipeline;
    GstElement *source;
    GstElement *decoder;
    GstElement *crop;
    GstElement *flip;
    GstElement *converter;
    GstElement *sink;
} CustomData;

int main(int argc, char *argv[])
{
    CustomData data;
    GstStateChangeReturn ret;
    gint flip_mode = 0;
    gint crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0;

    gst_init(&argc, &argv);

    data.pipeline = gst_pipeline_new("camera-pipeline");
    data.source = gst_element_factory_make("v4l2src", "source");
    data.decoder = gst_element_factory_make("jpegdec", "decoder");
    data.crop = gst_element_factory_make("videocrop", "crop");
    data.flip = gst_element_factory_make("videoflip", "flip");
    data.converter = gst_element_factory_make("videoconvert", "converter");
    data.sink = gst_element_factory_make("autovideosink", "sink");

    if (!data.pipeline || !data.source || !data.decoder || !data.crop || !data.flip || !data.converter || !data.sink)
    {
        g_printerr("Failed to create elements.\n");
        return -1;
    }

    g_object_set(data.source, "device", "/dev/video0", NULL);
    g_object_set(data.crop, "top", crop_top, "bottom", crop_bottom, "left", crop_left, "right", crop_right, NULL);
    g_object_set(data.flip, "method", flip_mode, NULL);

    gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.decoder, data.crop, data.flip, data.converter, data.sink, NULL);

    if (!gst_element_link_many(data.source, data.decoder, data.crop, data.flip, data.converter, data.sink, NULL))
    {
        g_printerr("Failed to link elements.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }

    ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }

    while (true)
    {
        char input;
        g_print("\nCommands:\n"
                "  f = flip\n"
                "  t = set crop top\n"
                "  b = set crop bottom\n"
                "  l = set crop left\n"
                "  r = set crop right\n"
                "  q = quit\n");
        g_print("Enter command: ");
        input = getchar();
        while (getchar() != '\n')
            ;

        switch (input)
        {
        case 'q':
            g_print("Quitting...\n");
            goto exit_loop;

        case 'f':
            flip_mode = (flip_mode + 1) % 4;
            g_object_set(data.flip, "method", flip_mode, NULL);
            g_print("Flip method changed to: %d\n", flip_mode);
            break;

        case 't':
        case 'b':
        case 'l':
        case 'r':
        {
            int value;
            g_print("Enter value: ");
            if (scanf("%d", &value) != 1)
            {
                g_print("Invalid input. Skipping.\n");
                while (getchar() != '\n')
                    ;
                break;
            }
            while (getchar() != '\n')
                ;

            switch (input)
            {
            case 't':
                crop_top = value;
                break;
            case 'b':
                crop_bottom = value;
                break;
            case 'l':
                crop_left = value;
                break;
            case 'r':
                crop_right = value;
                break;
            }

            g_object_set(data.crop,
                         "top", crop_top,
                         "bottom", crop_bottom,
                         "left", crop_left,
                         "right", crop_right,
                         NULL);

            g_print("Crop updated - top: %d, bottom: %d, left: %d, right: %d\n",
                    crop_top, crop_bottom, crop_left, crop_right);
            break;
        }

        default:
            g_print("Unknown command.\n");
            break;
        }
    }

exit_loop:
    gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(data.pipeline);

    return 0;
}
