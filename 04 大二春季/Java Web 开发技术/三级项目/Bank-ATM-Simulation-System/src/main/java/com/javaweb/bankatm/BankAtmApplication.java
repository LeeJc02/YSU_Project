package com.javaweb.bankatm;

import com.javaweb.bankatm.mapper.UserMapper;
import com.javaweb.bankatm.pojo.User;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.security.servlet.SecurityAutoConfiguration;

import java.util.List;


@SpringBootApplication(exclude = SecurityAutoConfiguration.class)
//@SpringBootApplication
public class BankAtmApplication {
    public static void main(String[] args) {
        SpringApplication.run(BankAtmApplication.class, args);
    }
}
