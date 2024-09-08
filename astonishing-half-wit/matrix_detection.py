from math import inf
from typing import List
import numpy as np
import cv2 as cv
import cv2
from pprint import pprint
from itertools import filterfalse as reject
from time import sleep
from models.configuration import Config

GREEN = (0, 255, 0)

#    0,    1,  2,      3
# next, prev, fc, parent


def find_inner_contours(heirarchy) -> List[int]:
    return filter(
        lambda x:
        x[1][2] == -1,
        enumerate(heirarchy[0])
    )


def draw_contours_on_image(contours, image):
    if len(contours) == 0:
        return image
    return cv.drawContours(
        image,
        list(zip(*contours))[1],
        -1,
        GREEN,
        thickness=2,
    )


def draw_points_on_image(points, image):
    for x, y in points:
        cv.circle(image, {x, y}, 1, GREEN, thickness=3)
        cv.putText(image, f"{x}, {y}", (x + 10, y), cv, 0.5, GREEN)
    return image


def if_quad(contour, precision: float):
    peri = cv.arcLength(contour, True)
    approx = cv.approxPolyDP(contour, precision * peri, True)
    return cv.convexHull(approx, returnPoints=False) == 4,


def find_quads(contours, precision: float) -> List[int]:
    return list(filter(lambda x: if_quad(x[1], precision), contours))


def find_inner_most_contours(heirarchy):
    return list(
        map(
            lambda x:
            x[0],
            filter(
                lambda h:
                h[1][2] == -1,
                enumerate(heirarchy)
            )
        )
    )


def find_contour_containg_matrix(heirarchy) -> List[int]:
    return list(
        set(
            map(
                lambda x:
                x[1][3],
                find_inner_contours(heirarchy)
            )
        )
    )


def filter_by_area(contours, min_area: int, max_area: int):
    f = list(
        reject(
            lambda x:
                cv.contourArea(x[1]) > max_area
                or cv.contourArea(x[1]) < min_area,
            contours)
    )
    return f


def find_extreme_points(contour):
    peri = cv.arcLength(contour, True)
    cv.approxPolyDP(contour, 0.02 * peri, True)


def decode_matrix(image, config: Config):

    gray = cv.cvtColor(image,  cv.COLOR_BGR2GRAY)
    blurred = cv.medianBlur(gray,  config.blur_size)

    if config.adaptive_threshold:
        thresholded = cv.adaptiveThreshold(
            blurred, config.threshold_max, cv.ADAPTIVE_THRESH_MEAN_C,
            cv.THRESH_BINARY_INV, config.block_size,  config.c)
    else:
        _, thresholded = cv.threshold(
            blurred,
            config.threshold,
            config.threshold_max,
            cv.THRESH_BINARY_INV
        )
    kernel_open = cv.getStructuringElement(cv.MORPH_RECT, (3, 3))
    kernel_close = cv.getStructuringElement(cv.MORPH_RECT, (5, 5))
    morph = cv.morphologyEx(thresholded, cv.MORPH_CLOSE, kernel_close)
    morph = cv.morphologyEx(morph, cv.MORPH_OPEN, kernel_open)

    contours, heirarchy = cv.findContours(
        morph,
        cv.RETR_TREE,
        cv.CHAIN_APPROX_NONE)

    contours = list(enumerate(contours))

    contours_with_valid_area = filter_by_area(
        contours,
        config.area_min,
        config.area_max,
    )
    quad_contours = find_quads(
        contours_with_valid_area,
        config.quad_precision)

    print(
        f"Total contours: {len(contours)}, area_filtered: {len(contours_with_valid_area)}, quads: {len(quad_contours)}")
    ret_val = {
        "morph": morph,
    }

    ret_val = {k: cv.cvtColor(v, cv.COLOR_GRAY2RGB)
               for k, v in ret_val.items()}
    ret_val["final"] = cv.cvtColor(
        draw_contours_on_image(quad_contours, image),
        cv.COLOR_BGR2RGB,
    )
    return ret_val


def main():
    image = cv.imread("./input_images/3.jpeg")
    decoded_image = decode_matrix(image)
    cv.imshow("frame", decoded_image)
    cv.waitKey()


def show_video():
    cap = cv.VideoCapture(0)
    while True:
        sleep(0.01)
        ret, image = cap.read()
        if ret:
            decoded = decode_matrix(image)
            if isinstance(decoded, list):
                for i, im in enumerate(decoded):
                    cv.imshow(f"frame {i}", im)
        else:
            pass
        cv.imshow("original", image)
        if cv.waitKey(1) == ord('q'):
            break


if __name__ == "__main__":
    show_video()
