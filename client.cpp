#include <gst/gst.h>

typedef struct _CustomData {
  GstElement *pipeline;
  GstElement *source;
  GstElement *decoder;
  GstElement *crop;
  GstElement *flip;
  GstElement *converter;
  GstElement *sink;
} CustomData;

int main(int argc, char *argv[]) {
    CustomData data;

    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    gst_init(&argc, &argv);

    data.pipeline = gst_pipeline_new("camera-pipeline");
    data.source = gst_element_factory_make("v4l2src", "source");
    data.decoder = gst_element_factory_make("jpegdec", "decoder");
    data.crop = gst_element_factory_make("videocrop", "crop");
    data.flip = gst_element_factory_make("videoflip", "flip");
    data.converter = gst_element_factory_make("videoconvert", "converter");
    data.sink = gst_element_factory_make("autovideosink", "sink");

    if (!data.pipeline || !data.source || !data.decoder || !data.crop || !data.flip || !data.converter || !data.sink) {
        g_printerr("Failed to create elements.\n");
        return -1;
    }

    g_object_set(data.source, "device", "/dev/video0", NULL);

    g_object_set(data.crop,
                 "top", 10,
                 "bottom", 20,
                 "left", 30,
                 "right", 20,
                 NULL);

    g_object_set(data.flip, "method", 0, NULL);

    gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.decoder, data.crop, data.flip, data.converter, data.sink, NULL);

    if (!gst_element_link_many(data.source, data.decoder, data.crop, data.flip, data.converter, data.sink, NULL)) {
        g_printerr("Failed to link elements.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }

    ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }

    bus = gst_element_get_bus(data.pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if (msg != NULL) {
        GError *err;
        gchar *debug_info;
        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Debugging info: %s\n", debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End-Of-Stream reached.\n");
                break;
            default:
                g_printerr("Unexpected message received.\n");
                break;
        }
        gst_message_unref(msg);
    }

    gst_object_unref(bus);
    gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(data.pipeline);

    return 0;
}