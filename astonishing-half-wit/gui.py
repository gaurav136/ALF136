import dearpygui.dearpygui as dpg
import cv2 as cv
import numpy as np
import matrix_detection
from models.configuration import Config


def convert_to_dpg(frame):
    data = frame.ravel()
    data = np.asfarray(data, dtype='f')
    texture_data = np.true_divide(data, 255.0)
    return texture_data


def main():

    config = Config.load_config()

    dpg.create_context()
    dpg.create_viewport(title='Custom Title', width=1000, height=800)
    dpg.setup_dearpygui()

    cap = cv.VideoCapture(0)
    ret, frame = cap.read()

    def set_exposure(ca, opt):
        cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 3)
        cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 1)
        cap.set(cv.CAP_PROP_EXPOSURE, opt)

    frame_width = cap.get(cv.CAP_PROP_FRAME_WIDTH)
    frame_height = cap.get(cv.CAP_PROP_FRAME_HEIGHT)

    # This popluates the default values for the windows
    data = np.flip(frame, 2)
    data = data.ravel()
    data = np.asfarray(data, dtype='f')
    texture_data = np.true_divide(data, 255.0)

    with dpg.texture_registry(show=True):
        dpg.add_raw_texture(frame_width, frame_height, texture_data,
                            tag="thresholded", format=dpg.mvFormat_Float_rgb)
        dpg.add_raw_texture(frame_width, frame_height, texture_data,
                            tag="final", format=dpg.mvFormat_Float_rgb)

    with dpg.window(label="Output Window", width=frame_width+50):
        dpg.add_text("Final")
        dpg.add_image("final")
        dpg.add_text("Thresholded")
        dpg.add_image("thresholded")

    with dpg.window(label="Configuration", pos=(frame_width+50, 0)):
        dpg.add_slider_int(
            label="Threshold",
            default_value=config.threshold,
            width=100,
            max_value=255,
            min_value=0,
            tag="threshold_value"
        )
        dpg.add_checkbox(
            label="Adaptive thresholding",
            tag="adaptive_threshold"
        )
        dpg.add_slider_int(
            label="BlockSize",
            default_value=config.block_size,
            width=100,
            max_value=50,
            min_value=0,
            tag="block_size",
        )
        dpg.add_slider_int(
            label="C",
            default_value=config.c,
            width=100,
            max_value=50,
            min_value=0,
            tag="c",
        )
        dpg.add_slider_int(
            label="Blur size",
            default_value=config.blur_size,
            width=100,
            max_value=25,
            min_value=2,
            tag="blur_size",
        )
        dpg.add_slider_int(
            label="Area Min",
            default_value=config.area_min,
            width=100,
            max_value=30_000,
            min_value=1_000,
            tag="area_min",
        )
        dpg.add_slider_int(
            label="Area Max",
            default_value=config.area_max,
            width=100,
            max_value=100_000,
            min_value=30_000,
            tag="area_max",
        )
        dpg.add_slider_float(
            label="Quad precision",
            default_value=config.quad_precision,
            width=100,
            max_value=1,
            min_value=0.0,
            tag="quad_precision",
        )
        dpg.add_slider_float(
            label="Exposure",
            default_value=200,
            width=100,
            max_value=2000,
            min_value=0.0,
            tag="exposure",
            callback=set_exposure,
        )

    dpg.show_metrics()
    dpg.show_viewport()
    cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 3)
    # cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 1)
    print(cap)
    while dpg.is_dearpygui_running():

        ret, frame = cap.read()
        config.threshold = dpg.get_value("threshold_value")
        config.block_size = dpg.get_value("block_size") * 2 + 3
        config.adaptive_threshold = dpg.get_value("adaptive_threshold")
        config.c = dpg.get_value("c")
        config.blur_size = dpg.get_value("blur_size")*2+3
        config.quad_precision = dpg.get_value("quad_precision")
        config.area_min = dpg.get_value("area_min")
        config.area_max = dpg.get_value("area_max")

        decoded = matrix_detection.decode_matrix(frame, config)
        decoded = {k: convert_to_dpg(v) for k, v in decoded.items()}
        dpg.set_value("thresholded", decoded["morph"])
        dpg.set_value("final", decoded["final"])
        # dpg.set_value("blurred", decoded["blurred"])
        dpg.render_dearpygui_frame()

    config.blur_size = (config.blur_size - 3)//2
    config.block_size = (config.block_size - 3)//2
    config.save_config()
    cap.release()
    dpg.destroy_context()


if __name__ == "__main__":
    main()
