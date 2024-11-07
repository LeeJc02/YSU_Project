import os
import subprocess
import re  # 导入正则表达式模块
from PIL import Image, ImageDraw, ImageFont

# 目录路径
programs_dir = './codes'
output_dir = './imgs'
code_imgs_path = output_dir + '/code_imgs'
effect_imgs_path = output_dir + '/effect_imgs'

# 创建保存输出的目录
if not os.path.exists(output_dir):
    os.makedirs(output_dir)
if not os.path.exists(code_imgs_path):
    os.makedirs(code_imgs_path)
if not os.path.exists(effect_imgs_path):
    os.makedirs(effect_imgs_path)

# 设置字体大小和行间距
font_size = 20  # 修改字体大小
line_spacing = 10  # 设置额外的行间距
try:
    font = ImageFont.truetype("arial.ttf", font_size)  # 可根据实际路径修改字体
except IOError:
    font = ImageFont.load_default()

# 函数：将代码转换为图像，并在第一行添加文件路径
def save_code_as_image(file_path, code_imgs_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    # 在第一行添加文件路径
    lines.insert(0, f"python {file_path}\n")
    lines.insert(1, "\n")

    # 图像尺寸估计
    line_height = font_size + line_spacing  # 每行的高度，包含额外的间距
    width = 800
    height = line_height * len(lines) + 40  # 计算总高度
    img = Image.new('RGB', (width, height), color=(0, 0, 0))  # 黑色背景
    draw = ImageDraw.Draw(img)

    # 在图像上绘制代码文本（白色文字）
    y_text = 10
    for line in lines:
        draw.text((10, y_text), line.strip(), font=font, fill=(255, 255, 255))  # 白色文字
        y_text += line_height  # 更新 y 坐标

    img.save(code_imgs_path)

# 函数：将输出结果转换为图像，并在第一行添加文件路径，最后添加 "Process finished with exit code 0"
def save_output_as_image(output_text, effect_imgs_path, script_path, user_input=None, prompt_text=None):
    lines = []

    # 在第一行添加文件路径
    lines.append(f" {script_path}")

    # 处理提示性语句和用户输入
    if prompt_text is not None:
        if user_input is not None:
            lines.append(prompt_text + user_input)  # 添加用户输入
        else:
            lines.append(prompt_text)  # 添加提示性语句

    # 检查 prompt_text 和 output_text 是否都不为空
    if prompt_text and output_text:
    # 如果 output_text 包含 prompt_text，则剔除它
        if prompt_text in output_text:
            output_text = output_text.replace(prompt_text, '').strip()
        
    # 添加程序的输出内容（不包含重复的提示信息）    
    lines.extend(output_text.splitlines())

    # 在结尾添加 "Process finished with exit code 0" 和一个空行
    lines.append("")
    lines.append("Process finished with exit code 0")

    # 图像尺寸估计
    line_height = font_size + line_spacing  # 每行的高度，包含额外的间距
    width = 800
    height = line_height * len(lines) + 40  # 计算总高度
    img = Image.new('RGB', (width, height), color=(0, 0, 0))  # 黑色背景
    draw = ImageDraw.Draw(img)

    # 在图像上绘制输出文本（白色文字）
    y_text = 10
    for line in lines:
        draw.text((10, y_text), line.strip(), font=font, fill=(255, 255, 255))  # 白色文字
        y_text += line_height  # 更新 y 坐标

    img.save(effect_imgs_path)

# 函数：检测输入提示语并获取提示内容
def get_input_prompt(file_path):
    prompt = None
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()
        # 查找所有的 input() 语句，获取提示性语句
        match = re.search(r'input\(\s*["\'](.*?)["\']\s*\)', content)
        if match:
            prompt = match.group(1)
    return prompt

# 函数：运行脚本并捕获输入和输出
def run_script_with_input(script_path, user_input):
    process = subprocess.Popen(['python', script_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    # 传递用户输入
    stdout, stderr = process.communicate(input=user_input)

    # 捕获输出并组合标准输出和标准错误
    output_text = stdout + '\n' + stderr
    return output_text

# 执行每个脚本并截图
for script in os.listdir(programs_dir):
    if script.endswith('.py'):
        script_path = os.path.join(programs_dir, script)
        script_name = os.path.splitext(script)[0]

        # 1. 保存代码截图（在第一行添加文件路径）
        code_image_file = os.path.join(code_imgs_path, f"{script_name}_code.png")
        save_code_as_image(script_path, code_image_file)
        print(f"Code screenshot saved for {script_name}")

        # 2. 检测输入提示语
        input_prompt = get_input_prompt(script_path)

        # 3. 如果检测到输入提示，要求用户输入
        user_input = ""
        if input_prompt:
            user_input = input(f"{script_name} {input_prompt} ")

        # 4. 运行 Python 脚本并捕获输入和输出
        print(f"Running {script_path} with input")
        output_text = run_script_with_input(script_path, user_input)

        # 5. 保存输出结果为图片（在第一行添加文件路径，最后添加 "Process finished with exit code 0"）
        output_image_file = os.path.join(effect_imgs_path, f"{script_name}_output.png")
        save_output_as_image(output_text, output_image_file, script_path, user_input, input_prompt)
        print(f"Output screenshot saved for {script_name}")
