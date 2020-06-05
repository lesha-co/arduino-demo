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

    # переводим в grayscale, затем применяем threshold, конвертим в ч/б,
    # потом ресайзим без сглаживания
    im_threshold: Image = ImageOps.grayscale(im).point(im_filter).convert(
        '1').resize((target_width, target_height), Image.NEAREST)

    # инвертируем
    im_threshold_inverted = ImageChops.invert(im_threshold)

    img_bytes = im_threshold_inverted.tobytes()

    # Вариант 1: Вывод бинарных литералов (B01010101)
    # Наглядно в коде (каждый бит = 1 пиксель), занимает больше места
    # bytestr = list(map(lambda x: 'B'+bin(x)[2:].zfill(8), img_bytes))

    # Вариант 2: Вывод hex литералов (0xff)
    # Занимает меньше места, но по коду не видно, что за картинка
    bytestr = list(map(lambda x: '0x'+hex(x)[2:].zfill(2), img_bytes))

    strings = []
    for i in range(0, len(bytestr), split_after):
        strings.append(', '.join(bytestr[i:i+split_after]) + ',')

    bytestr2 = '\n'.join(strings)

    # B00000000, B00000000, ....
    # или
    # 0x12, 0x34, ....
    # В С-коде для ардуино это будет выглядеть так:
    #
    #     static const unsigned char PROGMEM image[] =
    #     {
    #         /* вывод print()-a */
    #     };

    print(bytestr2)


if __name__ == "__main__":
    main()
