from PIL import Image, ImageOps, ImageChops
import math


def threshold(n):
    def fn(pixel):
        if pixel > n:
            return 255
        return 0

    return fn


def main():
    im_filter = threshold(240)

    im = Image.open("image.jpg")

    # ресайз с сохранением соотношения под нужную высоту
    target_height = 64
    target_width = round(im.width / (im.height/target_height))

    # настройка вывода: сколько байт выводить в одной строчке
    split_after = int(target_width/8)

    # переводим в grayscale, затем применяем threshold, потом ресайзим без сглаживания
    im_threshold: Image = ImageOps.grayscale(im).point(im_filter).convert(
        '1').resize((target_width, target_height), Image.NEAREST)

    # инвертируем
    im_threshold_inverted = ImageChops.invert(im_threshold)

    # переводим байты в строки побитово
    # 0xf0 => "B11110000"
    img_bytes = im_threshold_inverted.tobytes()
    bytestr = list(map(lambda x: 'B'+bin(x)[2:].zfill(8), img_bytes))

    strings = []
    for i in range(0, len(bytestr), split_after):
        strings.append(', '.join(bytestr[i:i+split_after]) + ',')

    bytestr2 = '\n'.join(strings)

    print(bytestr2)
    # B00000000, B00000000, ....

    # В С-коде для ардуино это будет выглядеть как
    # static const unsigned char PROGMEM image[] =
    # {
    #     B00000000, B00000000, ......};


if __name__ == "__main__":
    main()
