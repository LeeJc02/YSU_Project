new Vue({
    el: '#app',
    data: {
        users: [],           // 存储用户信息
        searchCardNumber: '' // 存储用户输入的卡号
    },
    created() {
        this.getAllUsers();  // 页面加载时获取所有用户信息
    },
    methods: {
        // 返回上一页
        goBack() {
            window.history.back();  // 返回上一页
        },

        // 查询所有用户信息
        getAllUsers() {
            axios.post('/admin/users')  // 使用POST请求来获取所有用户信息
                .then(response => {
                    this.users = response.data.data;  // 假设返回的数据是一个用户数组
                    this.reassignUserIds();  // 调用函数重新赋值用户ID
                })
                .catch(error => {
                    alert('获取用户信息失败，请稍后再试');
                });
        },

        // 根据卡号查询用户信息
        getUserByCardNumber() {
            if (!this.searchCardNumber) {
                alert('请输入卡号');
                return;
            }

            axios.post('/admin/user', { cardNumber: this.searchCardNumber })  // 根据输入的卡号查询
                .then(response => {
                    if (response.data.data) {
                        this.users = [response.data.data];  // 如果找到用户，更新数据
                    } else {
                        this.users = [];  // 如果没有找到，清空数据
                        alert('未找到该卡号的用户');
                    }
                })
                .catch(error => {
                    alert('查询失败，请稍后再试');
                });
        },

        // 重新设置用户ID为从1开始递增
        reassignUserIds() {
            this.users.forEach((user, index) => {
                user.userId = index + 1;  // 设置用户ID从1开始
            });
        }
    }
});
