package com.javaweb.bankatm.util;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

/**
 * @Classname DateAndStringUtil
 * @Description 关于时间的一些工具
 * @Date 2025/3/17 上午4:52
 * @Created by Lee
 */
public class DateAndStringUtil {
    /**
     * 将当前时间转换为字符串
     * @param
     * @return String
     */
    public static String formatToString() {
        LocalDateTime dateTime = LocalDateTime.now();
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
        return dateTime.format(formatter);
    }

    /**
     * 将传入时间转换为字符串
     * @param dateTime
     * @return String
     */
    public static String formatToString(LocalDateTime dateTime) {
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
        return dateTime.format(formatter);
    }
}
