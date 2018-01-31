import os
import re
import urllib.parse, urllib.request
import hashlib
import urllib
import random
import json
import time
# from translate import Translator

dir = r"D:\DevKit\msys32\home\xuanze\esp\esp-idf\tools\xuanze_translate"
# 1.获取当前目录下md文件
SRC = []
DST = []

url_google = 'http://translate.google.cn'
reg_text = re.compile(r'(?<=TRANSLATED_TEXT=).*?;')
user_agent = r'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) ' \
             r'Chrome/44.0.2403.157 Safari/537.36'


def translateGoogle(text, f='en', t='zh-cn'):
    values = {'hl': 'zh-cn', 'ie': 'utf-8', 'text': text, 'langpair': '%s|%s' % (f, t)}
    value = urllib.parse.urlencode(values)
    req = urllib.request.Request(url_google + '?' + value)
    req.add_header('User-Agent', user_agent)
    response = urllib.request.urlopen(req)
    content = response.read().decode('utf-8')
    data = reg_text.search(content)
    result = data.group(0).strip(';').strip('\'')

    return result


def file_name(file_dir):
    for root, dirs, files in os.walk(file_dir):
        for file in files:
            if os.path.splitext(file)[1] == '.md':
                SRC.append(os.path.join(root, file))
    return SRC


def trans_clear(src_line="none", zh_line="none"):
    zh_line = zh_line.replace("（", "(")
    zh_line = zh_line.replace("）", ")")
    zh_line = zh_line.replace("，", ",")
    zh_line = zh_line.replace("：", ":")
    zh_line = zh_line.replace("。", ".")

    # 去掉乱码
    zh_line = zh_line.replace("\\x3cbr\\x3e","\n")
    # 在*后添加空格
    zh_line = re.sub("^[*]", "* ", zh_line)
    # 在1.后添加空格
    if re.match("^[0-9][.]", zh_line):
        zh_line = zh_line.replace(".", ". ")
    # 从原文中查找''
    pattern = re.compile("[`\"“][^，,。：*\u4e00-\u9fa5]*[`\"”]")
    if pattern.search(zh_line) != None:
        zh_line = zh_line.replace("``", "`")
        zh_line = zh_line.replace("”", "`")
        zh_line = zh_line.replace("“", "`")
    print(zh_line)
    return zh_line


def file_save_zh(file_dir):
    file_name(file_dir)
    zhstr = None
    # 遍历md文件路径
    for str in SRC:
        c_src_flag = 0

        root, ext = os.path.splitext(str)
        # 如果文件路径中不包含zh
        if not "-ZH" in root:
            file = open(root + ext, "r", encoding='UTF-8')
            zh_file = open(root + '-ZH' + ext, "a+", encoding='UTF-8')
            # 逐行读取拷贝
            for line in file.readlines():
                copy_src_flag = False
                # 判断是否要copy源文件
                if "```" in line:
                    c_src_flag += 1
                    copy_src_flag = True
                if ("#" in line) or ((c_src_flag % 2) != 0) or ('\n' is line):
                    copy_src_flag = True
                #     按照标志位copy文件
                if copy_src_flag:
                    zh_file.write(line)
                #     需要翻译的行进行整理
                else:
                    zh_line = translateGoogle(line)
                    zh_line = trans_clear(line, zh_line)
                    zh_file.write(zh_line)
                # zh_file.flush()
            file.close()
            zh_file.close()
        else:
            continue
    return


if __name__ == '__main__':
    file_save_zh(dir)
