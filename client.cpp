#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *decoder, *converter, *sink;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create elements
    pipeline = gst_pipeline_new("camera-pipeline");
    source = gst_element_factory_make("v4l2src", "source");
    decoder = gst_element_factory_make("jpegdec", "decoder");
    converter = gst_element_factory_make("videoconvert", "converter");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !source || !decoder || !converter || !sink) {
        g_printerr("Failed to create elements.\n");
        return -1;
    }

    // Set properties
    g_object_set(source, "device", "/dev/video0", NULL);

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, decoder, converter, sink, NULL);


    // Link static elements
    if (!gst_element_link_many(source, decoder, converter, sink, NULL)) {
        g_printerr("Failed to link elements.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Start playback
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Wait for error or EOS
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    // Process messages
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

    // Cleanup
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}