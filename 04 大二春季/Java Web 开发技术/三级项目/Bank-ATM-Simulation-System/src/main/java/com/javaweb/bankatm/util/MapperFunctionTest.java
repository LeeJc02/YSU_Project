package com.javaweb.bankatm.util;

import com.javaweb.bankatm.mapper.TransactionMapper;
import com.javaweb.bankatm.mapper.UserMapper;
import com.javaweb.bankatm.pojo.Transaction;
import com.javaweb.bankatm.pojo.User;
import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

/**
 * @Classname MapperFunctionTest
 * @Description 用于Mapper数据库操作的函数测试
 * @Date 2025/3/17 上午4:40
 * @Created by Lee
 */
public class MapperFunctionTest {
    public static void main(String[] args) throws IOException {
        // 加载MyBatis配置文件
        String resource = "mybatis-config.xml";
        InputStream inputStream = Resources.getResourceAsStream(resource);
        // 构建 SqlSessionFactory
        SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        // 打开 SqlSession（默认非自动提交事务）
        SqlSession session = sqlSessionFactory.openSession();
        // 两种通过session获取到SQL方法
        List<User> userList1 = session.selectList("com.javaweb.bankatm.mapper.UserMapper.getUserList");
        UserMapper mapper = session.getMapper(UserMapper.class);
        List<User> userList2 = mapper.getUserList();


//            for (User user : userList) {
//                System.out.println(user);
//            }

//            User byCardAndPassword = mapper.findByCardAndPassword("6222021000000001", "123456");
//            System.out.println(byCardAndPassword);

//            List<Transaction> transactions = transactionMapper.getTransactionAll();

//            for (Transaction transaction : transactions) {
//                System.out.println(transaction);
//            }


    }
}
